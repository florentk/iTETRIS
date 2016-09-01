/**
 * \file DMS.hpp
 * \brief Header for GeographicLib::DMS class
 *
 * Copyright (c) Charles Karney (2008, 2009, 2010) <charles@karney.com>
 * and licensed under the LGPL.  For more information, see
 * http://geographiclib.sourceforge.net/
 **********************************************************************/

#if !defined(GEOGRAPHICLIB_DMS_HPP)
#define GEOGRAPHICLIB_DMS_HPP "$Id: DMS.hpp 6785 2010-01-05 22:15:42Z karney $"

#include "Constants.hpp"
#include <sstream>

namespace GeographicLib {

  /**
   * \brief Convert between degrees and %DMS representation.
   *
   * Parse a string representing degree, minutes, and seconds and return the
   * angle in degrees and format an angle in degrees as degree, minutes, and
   * seconds.
   **********************************************************************/
  class DMS {
  private:
    typedef Math::real real;
    static int lookup(const std::string& s, char c) throw() {
      std::string::size_type r = s.find(toupper(c));
      return r == std::string::npos ? -1 : int(r);
    }
    template<typename T> static std::string str(T x) {
      std::ostringstream s; s << x; return s.str();
    }
    static const std::string hemispheres;
    static const std::string signs;
    static const std::string digits;
    static const std::string dmsindicators;
    static const std::string components[3];
    DMS();                      // Disable constructor

  public:

    /**
     * Indicator for presence of hemisphere indicator (N/S/E/W) on latitudes
     * and longitudes.  AZIMUTH is used in Encode to indicate output in [000,
     * 360) with no letter indicator.
     **********************************************************************/
    enum flag { NONE = 0, LATITUDE = 1, LONGITUDE = 2, AZIMUTH = 3 };

    /**
     * Indicator for trailing units on an angle.
     **********************************************************************/
    enum component { DEGREE = 0, MINUTE = 1, SECOND = 2 };

    /**
     * Read a string \e dms in DMS format and return the resulting angle in
     * degrees.  Degrees, minutes, and seconds are indicated by the letters d,
     * ', ", and these components may only be given in this order.  Any (but
     * not all) components may be omitted.  The last component indicator may be
     * omitted and is assumed to be tbe next smallest unit (thus 33d10 is
     * interpreted as 33d10').  The final component may be a decimal fraction
     * but the non-final components must be integers.  The integer parts of the
     * minutes and seconds components must be less than 60.  A single leading
     * sign is permitted.  A hemisphere designator (N, E, W, S) may be added to
     * tbe beginning or end of the string.  The result is multiplied by the
     * implied signed of the hemisphere designator (negative for S and W).  In
     * addition \e flag is used to indicate whether such a designator was found
     * and whether it implies that the angle is a latitude (N or S) or
     * longitude (E or W).  Throws an error on a malformed string.  No check is
     * performed on the range of the result.
     **********************************************************************/
    static Math::real Decode(const std::string& dms, flag& ind);

    /**
     * Convert real degrees \e d, minutes \e m, and seconds \e s, to degrees.
     * This does not propagate the sign on \e d to the other components, so
     * -3d20' would need to be represented as - DMS::Decode(3.0, 20.0) or
     * DMS::Decode(-3.0, -20.0).
     **********************************************************************/
    static Math::real Decode(real d, real m = 0, real s = 0) throw()
    { return d + (m + s/real(60))/real(60); }

    /**
     * Convert two strings \e dmsa and \e dmsb to a latitude, \e lat, and
     * longitude, \e lon.  By default, the \e lat (resp., \e lon) is assigned
     * to the results of decoding \e dmsa (resp., \e dmsb).  However this is
     * overridden if either \e dmsa or \e dmsb contain a latitude or longitude
     * hemisphere designator (N, S, E, W).  Throws an error if the decoded
     * numbers are out of the ranges [-90<sup>o</sup>, 90<sup>o</sup>] for
     * latitude and [-180<sup>o</sup>, 360<sup>o</sup>] for longitude and, in
     * which case \e lat and \e lon are unchanged.  Finally the longitude is
     * reduced to the range [-180<sup>o</sup>, 180<sup>o</sup>).
     **********************************************************************/
    static void DecodeLatLon(const std::string& dmsa, const std::string& dmsb,
                             real& lat, real& lon);

    /**
     * Convert \e degree into a DMS string.  \e trailing indicates the least
     * significant component of the string (and this component is given as a
     * decimal number if necessary).  \e prec indicates the number of digits
     * after the decimal point for the trailing component.  \e flag indicates
     * additional formating as follows
     * - flag == NONE, signed result no leading zeros on degrees except in the
     *   units place, e.g., -8d03'.
     * - flag == LATITUDE, trailing N or S hemisphere designator, no sign, pad
     *   degress to 2 digits, e.g., 08d03'S.
     * - flag == LONGITUDE, trailing E or W hemisphere designator, no sign, pad
     *   degress to 3 digits, e.g., 008d03'W.
     * - flag == AZIMUTH, convert to the range [0, 360<sup>o</sup>), no sign,
     *   pad degrees to 3 digits, , e.g., 351d57'.
     * .
     * The integer parts of the minutes and seconds components are always given
     * with 2 digits.
     **********************************************************************/
    static std::string Encode(real degree, component trailing, unsigned prec,
                              flag ind = NONE);

    /**
     * Convert \e degree into a DMS string selecting the trailing component
     * based on \e prec.  \e prec indicates the precision relative to 1 degree,
     * e.g., \e prec = 3 gives a result accurate to 0.1' and \e prec = 4 gives
     * a result accurate to 1".
     **********************************************************************/
    static std::string Encode(real degree, unsigned prec, flag ind = NONE) {
      return Encode(degree,
                    prec < 2 ? DEGREE : (prec < 4 ? MINUTE : SECOND),
                    prec < 2 ? prec : (prec < 4 ? prec - 2 : prec - 4),
                    ind);
    }

    /**
     * Split angle, \e ang, into degrees, \e d, and minutes \e m.
     **********************************************************************/
    static void Encode(real ang, real& d, real& m) throw() {
      d = int(ang); m = 60 * (ang - d);
    }

    /**
     * Split angle, \e ang, into degrees, \e d, minutes, \e m, and seconds \e
     * s.
     **********************************************************************/
    static void Encode(real ang, real& d, real& m, real& s) throw() {
      d = int(ang); ang = 60 * (ang - d);
      m = int(ang); s = 60 * (ang - m);
    }

  };

} // namespace GeographicLib

#endif
