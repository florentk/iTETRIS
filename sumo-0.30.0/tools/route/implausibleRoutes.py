#!/usr/bin/env python
"""
@file    implausibleRoutes.py
@author  Jakob Erdmann
@date    2017-03-28
@version $Id: implausibleRoutes.py 23999 2017-04-21 09:04:47Z behrisch $

Find routes that are implausible due to:
 - being longer than the shortest path between the first and last edge 
 - being longer than the air-distance between the first and the last edge

The script computes an implausibility-score from configurable factors and
reports all routes above the specified threshold.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2014-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import sys
from optparse import OptionParser
import subprocess

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(os.path.join(tools))
    import sumolib
    from sumolib.output import parse, parse_fast
    from sumolib.net import readNet
    from sumolib.miscutils import Statistics, euclidean, Colorgen
    from route2poly import generate_poly
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")


def get_options():
    USAGE = """Usage %prog [options] <net.xml> <rou.xml>"""
    optParser = OptionParser(usage=USAGE)
    optParser.add_option("-v", "--verbose", action="store_true",
                         default=False, help="Give more output")
    optParser.add_option("--threshold", type="float", default=2.5,
                         help="Routes with an implausibility-score above treshold are reported")
    optParser.add_option("--airdist-ratio-factor", type="float", default=1, dest="airdist_ratio_factor",
                         help="Implausibility factor for the ratio of routeDist/airDist ")
    optParser.add_option("--detour-ratio-factor", type="float", default=1,  dest="detour_ratio_factor",
                         help="Implausibility factor for the ratio of routeDuration/shortestDuration ")
    optParser.add_option("--detour-factor", type="float", default=0.01, dest="detour_factor",
                         help="Implausibility factor for the absolute detour time in (routeDuration-shortestDuration) in seconds")
    optParser.add_option("--standalone", action="store_true",
                         default=False, help="Parse stand-alone routes that are not define as child-element of a vehicle")
    optParser.add_option("--blur", type="float", default=0,
                         help="maximum random disturbance to output polygon geometry")
    optParser.add_option("--ignore-routes", dest="ignore_routes",
                         help="List of route IDs (one per line) that are filtered when generating polygons and command line output (they will still be added to restrictions-output)")
    optParser.add_option("--restriction-output", dest="restrictions_output",
                         help="Write flow-restriction output suitable for passing to flowrouter.py to FILE")
    options, args = optParser.parse_args()

    if len(args) != 2:
        sys.exit(USAGE)
    options.network = args[0]
    options.routeFile = args[1]
    return options


def getRouteLength(net, edges):
    return sum([net.getEdge(e).getLength() for e in edges])


class RouteInfo:
    pass


def main():
    DUAROUTER = sumolib.checkBinary('duarouter')
    options = get_options()
    net = readNet(options.network)

    routeInfos = {}  # id-> RouteInfo
    if options.standalone:
        for route in parse(options.routeFile, 'route'):
            ri = RouteInfo()
            ri.edges = route.edges.split()
            routeInfos[route.id] = ri
    else:
        for vehicle in parse(options.routeFile, 'vehicle'):
            ri = RouteInfo()
            ri.edges = vehicle.route[0].edges.split()
            routeInfos[vehicle.id] = ri

    for rInfo in routeInfos.values():
        rInfo.airDist = euclidean(
            net.getEdge(rInfo.edges[0]).getShape()[0],
            net.getEdge(rInfo.edges[-1]).getShape()[-1])
        rInfo.length = getRouteLength(net, rInfo.edges)
        rInfo.airDistRatio = rInfo.length / rInfo.airDist

    duarouterInput = options.routeFile
    if options.standalone:
        # generate suitable input file for duarouter
        duarouterInput += ".vehRoutes.xml"
        with open(duarouterInput, 'w') as outf:
            outf.write('<routes>\n')
            for rID, rInfo in routeInfos.items():
                outf.write('    <vehicle id="%s" depart="0">\n' % rID)
                outf.write('        <route edges="%s"/>\n' % ' '.join(rInfo.edges))
                outf.write('    </vehicle>\n')
            outf.write('</routes>\n')

    duarouterOutput = options.routeFile + '.rerouted.rou.xml'
    duarouterAltOutput = options.routeFile + '.rerouted.rou.alt.xml'

    subprocess.call([DUAROUTER,
                     '-n', options.network,
                     '-r', duarouterInput,
                     '-o', duarouterOutput,
                     '--no-step-log'])

    for vehicle in parse(duarouterAltOutput, 'vehicle'):
        routeAlts = vehicle.routeDistribution[0].route
        if len(routeAlts) == 1:
            routeInfos[vehicle.id].detour = 0
            routeInfos[vehicle.id].detourRatio = 1
        else:
            oldCosts = float(routeAlts[0].cost)
            newCosts = float(routeAlts[1].cost)
            assert(routeAlts[0].edges.split() == routeInfos[vehicle.id].edges)
            if oldCosts <= newCosts:
                routeInfos[vehicle.id].detour = 0
                routeInfos[vehicle.id].detourRatio = 1
                if oldCosts < newCosts:
                    sys.stderr.write("Warning: fastest route for '%s' is slower than original route (old=%s, new=%s). Check vehicle types\n" % (
                        vehicle.id, oldCosts, newCosts))
            else:
                routeInfos[vehicle.id].detour = oldCosts - newCosts
                routeInfos[vehicle.id].detourRatio = oldCosts / newCosts

    implausible = []
    allRoutesStats = Statistics("overal implausiblity")
    implausibleRoutesStats = Statistics("implausiblity above threshold")
    for rID in sorted(routeInfos.keys()):
        ri = routeInfos[rID]
        ri.implausibility = (options.airdist_ratio_factor * ri.airDistRatio
                             + options.detour_factor * ri.detour
                             + options.detour_ratio_factor * ri.detourRatio)
        allRoutesStats.add(ri.implausibility, rID)
        if ri.implausibility > options.threshold:
            implausible.append((ri.implausibility, rID, ri))
            implausibleRoutesStats.add(ri.implausibility, rID)

    # generate restrictions
    if options.restrictions_output is not None:
        with open(options.restrictions_output, 'w') as outf:
            for score, rID, ri in sorted(implausible):
                outf.write("0 %s\n" % " ".join(ri.edges))

    if options.ignore_routes is not None:
        numImplausible = len(implausible)
        ignored = set([r.strip() for r in open(options.ignore_routes)])
        implausible = [r for r in implausible if not r in ignored]
        print("Loadeded %s routes to ignore. Reducing implausible from %s to %s" % (
            len(ignored), numImplausible, len(implausible)))

    # generate polygons
    polyOutput = options.routeFile + '.implausible.add.xml'
    colorgen = Colorgen(("random", 1, 1))
    with open(polyOutput, 'w') as outf:
        outf.write('<additional>\n')
        for score, rID, ri in sorted(implausible):
            generate_poly(net, rID, colorgen(), 100, False, ri.edges, options.blur, outf, score)
        outf.write('</additional>\n')

    for score, rID, ri in sorted(implausible):
        # , ' '.join(ri.edges)))
        sys.stdout.write('%s\t%s\t%s\n' % (score, rID, (ri.airDistRatio, ri.detourRatio, ri.detour)))

    print(allRoutesStats)
    print(implausibleRoutesStats)


if __name__ == "__main__":
    main()
