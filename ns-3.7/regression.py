# python lib modules
import os
import sys
import shutil
import pproc as subprocess
import errno

# WAF modules
import Options
import Utils
import Task

# local modules
import wutils


def dev_null():
    if sys.platform == 'win32':
        return open("NUL:", "w")
    else:
        return open("/dev/null", "w")


def _find_tests(testdir):
    """Return a list of test modules in the test directory

    Arguments:
    testdir -- the directory to look in for tests
    """
    names = os.listdir(testdir)
    tests = []
    for name in names:
        if name[:5] == "test-" and name[-3:] == ".py":
            testname = name[:-3]
            tests.append(testname)
    tests.sort()
    return tests

def diff(dir1, dir2, verbose):
    import filecmp
    comp = filecmp.dircmp(dir1, dir2)
    differ = (comp.left_only or comp.right_only or comp.diff_files)

    if differ:
        # ok, stupid binary comparison reports differences, but maybe
        # only text files differ, in which case we should compare
        # again while ignoring newline differences between
        # windows/mac/unix.
        if not comp.left_only and not comp.right_only:
            for diff_fname in comp.diff_files:
                if not (diff_fname.endswith(".tr") or diff_fname.endswith(".mob")):
                    # doesn't look like a text file; it has to differ
                    break
                diff_file1 = open(os.path.join(dir1, diff_fname), "rtU").readlines()
                diff_file2 = open(os.path.join(dir2, diff_fname), "rtU").readlines()
                if diff_file1 != diff_file2:
                    break
                #else:
                #    print ">>>>>>>> %s file does not really differ!" % (diff_fname)
            else:
                differ = False

    if differ:
        if verbose:
            comp.report()
            import difflib
            for diff_fname in comp.diff_files:
                if not (diff_fname.endswith(".tr") or diff_fname.endswith(".mob")):
                    print "The different file %r does not sound like a text file, not compared." % (diff_fname,)
                diff_file1 = open(os.path.join(dir1, diff_fname), "rt").readlines()
                diff_file2 = open(os.path.join(dir2, diff_fname), "rt").readlines()
                diff = difflib.unified_diff(diff_file1, diff_file2)
                count = 0
                print "Differences in file %r" % (diff_fname,)
                for line in diff:
                    print line
                    count += 1
                    if count > 100:
                        break
        return 1
    else:
        return 0

class regression_test_task(Task.TaskBase):
    after = 'cc cxx cc_link cxx_link'
    color = 'BLUE'

    def __init__(self, bld, env, test_name, test_scripts_dir, build_traces_dir, reference_traces):
        self.bld = bld
        self.generator = self
        self.env = env
        super(regression_test_task, self).__init__(generator=self, env=env)
        self.test_name = test_name

        assert self.test_name.startswith('test-')
        short_name = self.test_name[len('test-'):]

        self.test_scripts_dir = test_scripts_dir
        self.build_traces_dir = build_traces_dir
        self.reference_traces_dir = reference_traces

        sys.path.insert(0, self.test_scripts_dir)
        try:
            mod = __import__(self.test_name, globals(), locals(), [])
        finally:
            sys.path.remove(self.test_scripts_dir)
        self.mod = mod
        if hasattr(mod, 'may_run'):
            reason_cannot_run = mod.may_run(self.env, Options.options)
        else:
            reason_cannot_run = None
        if not reason_cannot_run:
            pyscript = getattr(mod, "pyscript", None)
            if pyscript:
                Options.options.compile_targets += ',ns3module,pybindgen-command'
            else:
                program = getattr(mod, "program", short_name)
                Options.options.compile_targets += ',' + program

    def __str__(self):
        return 'regression-test (%s)\n' % self.test_name

    def runnable_status(self):
        return Task.RUN_ME

    def run(self):
        """Run a single test"""
        assert self.test_name.startswith('test-')
        short_name = self.test_name[len('test-'):]
        mod = self.mod
        trace_dir_name = getattr(mod, "trace_dir_name", None)
        if trace_dir_name is None:
            trace_dir_name = "%s.ref" % short_name
        trace_output_path = os.path.join(self.build_traces_dir, trace_dir_name)
        reference_traces_path = os.path.join(self.reference_traces_dir, trace_dir_name)

        if hasattr(mod, 'get_arguments'):
            arguments = mod.get_arguments(self.env, '..')
        else:
            arguments = getattr(mod, "arguments", [])

        pyscript = getattr(mod, "pyscript", None)
        if pyscript:
            is_pyscript = True
            program = pyscript
        else:
            is_pyscript = False
            program = getattr(mod, "program", short_name)

        if hasattr(mod, 'may_run'):
            reason_cannot_run = mod.may_run(self.env, Options.options)
        else:
            reason_cannot_run = None
        if reason_cannot_run:
            print "SKIP %s (%s)" % (self.test_name, reason_cannot_run)
            self.result = None
            return 0

        if Options.options.regression_generate:
            # clean the target dir
            try:
                shutil.rmtree(reference_traces_path)
            except OSError, ex:
                if ex.errno not in [errno.ENOENT]:
                    raise
            os.makedirs(reference_traces_path)
            result = self.run_reference_generate(reference_traces_path, program, arguments, is_pyscript)
            if result == 0:
                print "GENERATE " + self.test_name
            else:
                print "GENERATE FAIL " + self.test_name
        else:
            # clean the target dir
            try:
                shutil.rmtree(trace_output_path)
            except OSError, ex:
                if ex.errno not in [errno.ENOENT]:
                    raise
            os.makedirs(trace_output_path)
            # run it
            #print "self.run_reference_test:(%r, %r, %r, %r, %r)" \
            #    % (reference_traces_path, trace_output_path, program, arguments, is_pyscript)
            result = self.run_reference_test(reference_traces_path, trace_output_path, program, arguments, is_pyscript)
            if result == 0:
                print "PASS " + self.test_name
            else:
                print "FAIL " + self.test_name
        self.result = result
        return 0

    def run_reference_test(self, reference_traces_path, trace_output_path, program, arguments, is_pyscript):
        if not os.path.exists(reference_traces_path):
            print "Cannot locate reference traces in " + reference_traces_path
            return 1

        if is_pyscript:
            script = os.path.abspath(os.path.join('..', *os.path.split(program)))
            argv = [self.env['PYTHON'], script] + arguments
            try:
                wutils.run_argv(argv, self.env, cwd=trace_output_path, force_no_valgrind=True)
            except Utils.WafError, ex:
                print >> sys.stderr, ex
                return 1
        else:
            try:
                wutils.run_program(program, self.env,
                                   command_template=wutils.get_command_template(self.env, arguments),
                                   cwd=trace_output_path)
            except Utils.WafError, ex:
                print >> sys.stderr, ex
                return 1

        rc = diff(trace_output_path, reference_traces_path, Options.options.verbose)
        if rc:
            print "----------"
            print "Traces differ in test: ", self.test_name
            print "Reference traces in directory: " + reference_traces_path
            print "Traces in directory: " + trace_output_path
            print "Run the following command for details:"
            print "\tdiff -u %s %s" % (reference_traces_path, trace_output_path)
            if not Options.options.verbose:
                print "Or re-run regression testing with option -v"
            print "----------"
        return rc


    def run_reference_generate(self, trace_output_path, program, arguments, is_pyscript):
        if is_pyscript:
            script = os.path.abspath(os.path.join('..', *os.path.split(program)))
            argv = [self.env['PYTHON'], script] + arguments
            try:
                retval = wutils.run_argv(argv, self.env, cwd=trace_output_path, force_no_valgrind=True)
            except Utils.WafError, ex:
                print >> sys.stderr, ex
                return 1
        else:
            try:
                retval = wutils.run_program(program, self.env,
                                            command_template=wutils.get_command_template(self.env, arguments),
                                            cwd=trace_output_path)
            except Utils.WafError, ex:
                print >> sys.stderr, ex
                return 1
        return retval


class regression_test_collector_task(Task.TaskBase):
    after = 'regression_test_task'
    color = 'BLUE'

    def __init__(self, bld, test_tasks):
        self.bld = bld
        super(regression_test_collector_task, self).__init__(generator=self)
        self.test_tasks = test_tasks

    def __str__(self):
        return 'regression-test-collector\n'

    def runnable_status(self):
        return Task.RUN_ME

    def run(self):
        failed_tests = [test for test in self.test_tasks if test.result is not None and test.result != 0]
        skipped_tests = [test for test in self.test_tasks if test.result is None]
        print "Regression testing summary:"
        if skipped_tests:
            print "SKIP: %i of %i tests have been skipped (%s)" % (
                len(skipped_tests), len(self.test_tasks),
                ', '.join([test.test_name for test in skipped_tests]))
        if failed_tests:
            print "FAIL: %i of %i tests have failed (%s)" % (
                len(failed_tests), len(self.test_tasks),
                ', '.join([test.test_name for test in failed_tests]))
            return 1
        else:
            print "PASS: %i of %i tests passed" % (len(self.test_tasks) - len(skipped_tests),
                                                   len(self.test_tasks))
            return 0

def run_regression(bld, reference_traces):
    """Execute regression tests.  Called with cwd set to the 'regression' subdir of ns-3.

    @param reference_traces: reference traces directory.

    """

    testdir = os.path.join("regression", "tests")
    if not os.path.exists(testdir):
        print "Tests directory does not exist"
        sys.exit(3)

    if Options.options.regression_tests:
        tests = Options.options.regression_tests.split(',')
    else:
        tests = _find_tests(testdir)

    if not os.path.exists(reference_traces):
        print "Reference traces directory (%s) does not exist" % reference_traces
        return 3
    
    test_scripts_dir = bld.path.find_dir('regression/tests').abspath()
    build_traces_dir = bld.path.find_or_declare('regression/traces').abspath(bld.env)
    tasks = []
    for test in tests:
        task = regression_test_task(bld, bld.env, test, test_scripts_dir, build_traces_dir, reference_traces)
        #bld.task_manager.add_task(task)
        tasks.append(task)
    regression_test_collector_task(bld, tasks)
