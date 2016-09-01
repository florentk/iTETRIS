/****************************************************************************/
/// @file    OutputDevice.h
/// @author  Daniel Krajzewicz
/// @date    2004
/// @version $Id: OutputDevice.h 11095 2011-08-10 18:07:09Z behrisch $
///
// Static storage of an output device and its base (abstract) implementation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef OutputDevice_h
#define OutputDevice_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <map>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <utils/xml/SUMOXMLDefinitions.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class OutputDevice
 * @brief Static storage of an output device and its base (abstract) implementation
 *
 * OutputDevices are basically a capsule around an std::ostream, which give a
 *  unified access to sockets, files and stdout.
 *
 * Usually, an application builds as many output devices as needed. Each
 *  output device may also be used to save outputs from several sources
 *  (several detectors, for example). Building is done using OutputDevice::getDevice()
 *  what also parses the given output description in order to decide
 *  what kind of an OutputDevice shall be built. OutputDevices are
 *  closed via OutputDevice::closeAll(), normally called at the application's
 *  end.
 *
 * Although everything that can be written to a stream can also be written
 *  to an OutputDevice, there is special support for XML tags (remembering
 *  all open tags to close them at the end). OutputDevices are still lacking
 *  support for function pointers with the '<<' operator (no endl, use '\n').
 *  The most important method to implement in subclasses is getOStream,
 *  the most used part of the interface is the '<<' operator.
 *
 * The Boolean markers are used rarely and might get removed in future versions.
 */
class OutputDevice {
public:
    /// @name static access methods to OutputDevices
    /// @{

    /** @brief Returns the described OutputDevice
     *
     * Creates and returns the named device. "stdout" and "-" refer to standard out,
     * "hostname:port" initiates socket connection. Otherwise a filename
     * is assumed and the second parameter may be used to give a base directory.
     * If there already is a device with the same name this one is returned.
     *
     * @param[in] name The description of the output name/port/whatever
     * @param[in] base The base path the application is run within
     * @return The corresponding (built or existing) device
     * @exception IOError If the output could not be built for any reason (error message is supplied)
     */
    static OutputDevice& getDevice(const std::string &name,
                                   const std::string &base="") throw(IOError);


    /** @brief Creates the device using the output definition stored in the named option
     *
     * Creates and returns the device named by the option. Asks whether the option
     *  and retrieves the name from the option if so. Optionally the XML header
     *  gets written as well. Returns whether a device was created (option was set).
     *
     * Please note, that we do not have to consider the "application base" herein,
     *  because this call is only used to get file names of files referenced
     *  within XML-declarations of structures which paths already is aware of the
     *  cwd.
     *
     * @param[in] optionName The name of the option to use for retrieving the output definition
     * @param[in] rootElement The root element to use (XML-output)
     * @return Whether a device was built (the option was set)
     * @exception IOError If the output could not be built for any reason (error message is supplied)
     */
    static bool createDeviceByOption(const std::string &optionName,
                                     const std::string &rootElement="") throw(IOError);


    /** @brief Returns the device described by the option
     *
     * Returns the device named by the option. If the option is unknown, unset
     * or the device was not created before, InvalidArgument is thrown.
     *
     * Please note, that we do not have to consider the "application base" herein.
     *
     * @param[in] name The name of the option to use for retrieving the output definition
     * @return The corresponding (built or existing) device
     * @exception IOError If the output could not be built for any reason (error message is supplied)
     * @exception InvalidArgument If the option with the given name does not exist
     */
    static OutputDevice& getDeviceByOption(const std::string &name) throw(IOError, InvalidArgument);


    /**  Closes all registered devices
     */
    static void closeAll() throw();
    /// @}


    /** @brief Abstract output operator
     * @return The OutputDevice for further processing
     */
    static std::string realString(const SUMOReal v, const int precision=OUTPUT_ACCURACY);



public:
    /// @name OutputDevice member methods
    /// @{

    /// @brief Constructor
    OutputDevice(const unsigned int defaultIndentation=0) throw(IOError);


    /// @brief Destructor
    virtual ~OutputDevice() throw() { }


    /** @brief returns the information whether one can write into the device
     * @return Whether the device can be used (stream is good)
     */
    virtual bool ok() throw();


    /** @brief Closes the device and removes it from the dictionary
     */
    void close() throw();


    /** @brief Sets the precison or resets it to default
     * @param[in] precision The accuracy (number of digits behind '.') to set
     */
    void setPrecision(unsigned int precision=OUTPUT_ACCURACY) throw();


    /** @brief Writes an XML header with optional configuration
     *
     * If something has been written (myXMLStack is not empty), nothing
     *  is written and false returned.
     *
     * @param[in] rootElement The root element to use
     * @param[in] xmlParams Additional parameters (such as encoding) to include in the <?xml> declaration
     * @param[in] attrs Additional attributes to save within the rootElement
     * @param[in] comment Additional comment (saved in front the rootElement)
     * @todo Check which parameter is used herein
     * @todo Describe what is saved
     */
    bool writeXMLHeader(const std::string &rootElement,
                        const std::string xmlParams="",
                        const std::string &attrs="",
                        const std::string &comment="") throw();


    /** @brief Adds indentation
     *
     * An intendation, depending on the current xml-element-stack size, is written.
     *
     * @returns The OutputDevice for further processing
     */
    OutputDevice& indent() throw();


    /** @brief Opens an XML tag
     *
     * An indentation, depending on the current xml-element-stack size, is written followed
     *  by the given xml element ("<" + xmlElement)
     * The xml element is added to the stack, then.
     *
     * @param[in] xmlElement Name of element to open
     * @returns The OutputDevice for further processing
     */
    OutputDevice& openTag(const std::string &xmlElement) throw();


    /** @brief Opens an XML tag
     *
     * Helper method which finds the correct string before calling openTag.
     *
     * @param[in] xmlElement Id of the element to open
     * @returns The OutputDevice for further processing
     */
    OutputDevice& openTag(const SumoXMLTag &xmlElement) throw();


    /** @brief Closes the most recently opened tag
     *
     * The topmost xml-element from the stack is written into the stream
     *  as a closing element ("</" + element + ">") and is then removed from
     *  the stack. If abbreviated closing is requested, only "/>" is the output.
     *
     * @param[in] name whether abbreviated closing is performed
     * @returns Whether a further element existed in the stack and could be closed
     * @todo it is not verified that the topmost element was closed
     */
    bool closeTag(bool abbreviated=false) throw();

    /** @brief writes an arbitrary attribute
     *
     * @param[in] attr The attribute (name)
     * @param[in] val The attribute value
     */
    OutputDevice &writeAttr(std::string attr, std::string val) {
        getOStream() << " " << attr << "=\"" << val << "\"";
        return *this;
    }

    /** @brief writes an named attribute
     *
     * @param[in] attr The attribute (name)
     * @param[in] val The attribute value
     */
    template <class T>
    OutputDevice &writeAttr(const SumoXMLAttr attr, const T &val) {
        return writeAttr(toString(attr), toString(val));
    }


    /** @brief Retrieves a message to this device.
     *
     * Implementation of the MessageRetriever interface. Writes the given message to the output device.
     *
     * @param[in] msg The msg to write to the device
     */
    void inform(const std::string &msg);


    /** @brief Abstract output operator
     * @return The OutputDevice for further processing
     */
    template <class T>
    OutputDevice &operator<<(const T &t) {
        getOStream() << t;
        postWriteHook();
        return *this;
    }

protected:
    /// @brief Returns the associated ostream
    virtual std::ostream &getOStream() throw() = 0;


    /** @brief Called after every write access.
     *
     * Default implementation does nothing.
     */
    virtual void postWriteHook() throw();


private:
    /// @name static members
    /// @{

    /// @brief Definition of a map from names to output devices
    typedef std::map<std::string, OutputDevice*> DeviceMap;

    /// @brief map from names to output devices
    static DeviceMap myOutputDevices;
    /// @}


private:
    /// @name Non-static members of each OutputDevice
    /// @{

    /// @brief The stack of begun xml elements
    std::vector<std::string> myXMLStack;

    /// @brief The initial indentation level
    unsigned int myDefaultIndentation;
    /// @}

};


#endif

/****************************************************************************/

