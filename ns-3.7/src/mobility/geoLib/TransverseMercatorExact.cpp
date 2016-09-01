/**
 * \file TransverseMercatorExact.cpp
 * \brief Implementation for GeographicLib::TransverseMercatorExact class
 *
 * Copyright (c) Charles Karney (2008, 2009, 2010) <charles@karney.com>
 * and licensed under the LGPL.  For more information, see
 * http://geographiclib.sourceforge.net/
 *
 * The relevant section of Lee's paper is part V, pp 67&ndash;101,
 * <a href="http://dx.doi.org/10.3138/X687-1574-4325-WM62">Conformal
 * Projections Based On Jacobian Elliptic Functions</a>.
 *
 * The method entails using the Thompson Transverse Mercator as an
 * intermediate projection.  The projections from the intermediate
 * coordinates to [\e phi, \e lam] and [\e x, \e y] are given by elliptic
 * functions.  The inverse of these projections are found by Newton's method
 * with a suitable starting guess.
 *
 * This implementation and notation closely follows Lee, with the following
 * exceptions:
 * <center><table>
 * <tr><th>Lee    <th>here    <th>Description
 * <tr><td>x/a    <td>xi      <td>Northing (unit Earth)
 * <tr><td>y/a    <td>eta     <td>Easting (unit Earth)
 * <tr><td>s/a    <td>sigma   <td>xi + i * eta
 * <tr><td>y      <td>x       <td>Easting
 * <tr><td>x      <td>y       <td>Northing
 * <tr><td>k      <td>e       <td>eccentricity
 * <tr><td>k^2    <td>mu      <td>elliptic function parameter
 * <tr><td>k'^2   <td>mv      <td>elliptic function complementary parameter
 * <tr><td>m      <td>k       <td>scale
 * </table></center>
 *
 * Minor alterations have been made in some of Lee's expressions in an
 * attempt to control round-off.  For example atanh(sin(phi)) is replaced by
 * asinh(tan(phi)) which maintains accuracy near phi = pi/2.  Such changes
 * are noted in the code.
 **********************************************************************/

#include "TransverseMercatorExact.hpp"

#define GEOGRAPHICLIB_TRANSVERSEMERCATOREXACT_CPP "$Id: TransverseMercatorExact.cpp 6785 2010-01-05 22:15:42Z karney $"

RCSID_DECL(GEOGRAPHICLIB_TRANSVERSEMERCATOREXACT_CPP)
RCSID_DECL(GEOGRAPHICLIB_TRANSVERSEMERCATOREXACT_HPP)

namespace GeographicLib {

  using namespace std;

  const Math::real TransverseMercatorExact::tol =
    numeric_limits<real>::epsilon();
  const Math::real TransverseMercatorExact::tol1 = real(0.1) * sqrt(tol);
  const Math::real TransverseMercatorExact::tol2 = real(0.1) * tol;
  const Math::real TransverseMercatorExact::taytol = pow(tol, real(0.6));
  // Overflow value for asinh(tan(pi/2)) etc.
  const Math::real TransverseMercatorExact::ahypover =
    real(numeric_limits<real>::digits) * log(real(numeric_limits<real>::radix))
    + 2;

  TransverseMercatorExact::TransverseMercatorExact(real a, real r, real k0,
                                                   bool extendp)
    : _a(a)
    , _r(r)
    , _f(1 / _r)
    , _k0(k0)
    , _mu(_f * (2 - _f))        // e^2
    , _mv(1 - _mu)              // 1 - e^2
    , _e(sqrt(_mu))
    , _ep2(_mu / _mv)           // e^2 / (1 - e^2)
    , _extendp(extendp)
    , _Eu(_mu)
    , _Ev(_mv)
  {
    if (!(_a > 0))
      throw GeographicErr("Major radius is not positive");
    if (!(_r > 0))
      throw GeographicErr("Inverse flattening is not positive");
    if (!(_k0 > 0))
      throw GeographicErr("Scale is not positive");
  }

  const TransverseMercatorExact
  TransverseMercatorExact::UTM(Constants::WGS84_a(), Constants::WGS84_r(),
                               Constants::UTM_k0());

  Math::real TransverseMercatorExact::psi(real phi) const throw() {
    real s = sin(phi);
    // Lee 9.4.  Rewrite atanh(sin(phi)) = asinh(tan(phi)) which is more
    // accurate.  Write tan(phi) this way to ensure that sign(tan(phi)) =
    // sign(phi)
    return Math::asinh(s / max(cos(phi), real(0.1) * tol)) -
      _e * Math::atanh(_e * s);
  }

  Math::real TransverseMercatorExact::psiinv(real psi) const throw() {
    // This is the inverse of psi.  Use Newton's method to solve for q in
    //
    //   psi = q - e * atanh(e * tanh(q))
    //
    // and then substitute phi = atan(sinh(q)).  Note that
    // dpsi/dq = (1 - e^2)/(1 - e^2 * tanh(q)^2)
    real q = psi;               // Initial guess
    for (int i = 0; i < numit; ++i) {
      // min iterations = 1, max iterations = 3; mean = 2.8
      real
        t = tanh(q),
        dq = -(q - _e * Math::atanh(_e * t) - psi) * (1 - _mu * sq(t)) / _mv;
      q += dq;
      if (abs(dq) < tol1)
        break;
    }
    return atan(sinh(q));
  }

  void TransverseMercatorExact::zeta(real u, real snu, real cnu, real dnu,
                                     real v, real snv, real cnv, real dnv,
                                     real& psi, real& lam) const throw() {
    // Lee 54.17 but write
    // atanh(snu * dnv) = asinh(snu * dnv / sqrt(cnu^2 + _mv * snu^2 * snv^2))
    // atanh(_e * snu / dnv) =
    //         asinh(_e * snu / sqrt(_mu * cnu^2 + _mv * cnv^2))
    real
      d1 = sqrt(sq(cnu) + _mv * sq(snu * snv)),
      d2 = sqrt(_mu * sq(cnu) + _mv * sq(cnv));
    psi =
      // Overflow to values s.t. tanh = 1.
      (d1 ? Math::asinh(snu * dnv / d1) : snu < 0 ? -ahypover : ahypover)
      - (d2 ? _e * Math::asinh(_e * snu / d2) : snu < 0 ? -ahypover : ahypover);
    lam = (d1 != 0 && d2 != 0) ?
      atan2(dnu * snv, cnu * cnv) - _e * atan2(_e * cnu * snv, dnu * cnv) :
      0;
  }

  void TransverseMercatorExact::dwdzeta(real u, real snu, real cnu, real dnu,
                                        real v, real snv, real cnv, real dnv,
                                        real& du, real& dv) const throw() {
    // Lee 54.21 but write (1 - dnu^2 * snv^2) = (cnv^2 + _mu * snu^2 * snv^2)
    // (see A+S 16.21.4)
    real d = _mv * sq(sq(cnv) + _mu * sq(snu * snv));
    du =  cnu * dnu * dnv * (sq(cnv) - _mu * sq(snu * snv)) / d;
    dv = -snu * snv * cnv * (sq(dnu * dnv) + _mu * sq(cnu)) / d;
  }

  // Starting point for zetainv
  bool TransverseMercatorExact::zetainv0(real psi, real lam, real& u, real& v)
    const throw() {
    bool retval = false;
    if (psi < -_e * Constants::pi()/4 &&
        lam > (1 - 2 * _e) * Constants::pi()/2 &&
        psi < lam - (1 - _e) * Constants::pi()/2) {
      // N.B. this branch is normally not taken because psi < 0 is converted
      // psi > 0 by Forward.
      //
      // There's a log singularity at w = w0 = Eu.K() + i * Ev.K(),
      // corresponding to the south pole, where we have, approximately
      //
      //   psi = _e + i * pi/2 - _e * atanh(cos(i * (w - w0)/(1 + _mu/2)))
      //
      // Inverting this gives:
      real
        psix = 1 - psi / _e,
        lamx = (Constants::pi()/2 - lam) / _e;
      u = Math::asinh(sin(lamx) / Math::hypot(cos(lamx), sinh(psix))) *
        (1 + _mu/2);
      v = atan2(cos(lamx), sinh(psix)) * (1 + _mu/2);
      u = _Eu.K() - u;
      v = _Ev.K() - v;
    } else if (psi < _e * Constants::pi()/2 &&
               lam > (1 - 2 * _e) * Constants::pi()/2) {
      // At w = w0 = i * Ev.K(), we have
      //
      //     zeta = zeta0 = i * (1 - _e) * pi/2
      //     zeta' = zeta'' = 0
      //
      // including the next term in the Taylor series gives:
      //
      // zeta = zeta0 - (_mv * _e) / 3 * (w - w0)^3
      //
      // When inverting this, we map arg(w - w0) = [-90, 0] to
      // arg(zeta - zeta0) = [-90, 180]
      real
        dlam = lam - (1 - _e) * Constants::pi()/2,
        rad = Math::hypot(psi, dlam),
        // atan2(dlam-psi, psi+dlam) + 45d gives arg(zeta - zeta0) in range
        // [-135, 225).  Subtracting 180 (since multiplier is negative) makes
        // range [-315, 45).  Multiplying by 1/3 (for cube root) gives range
        // [-105, 15).  In particular the range [-90, 180] in zeta space maps
        // to [-90, 0] in w space as required.
        ang = atan2(dlam-psi, psi+dlam) - real(0.75) * Constants::pi();
      // Error using this guess is about 0.21 * (rad/e)^(5/3)
      retval = rad < _e * taytol;
      rad = pow(3 / (_mv * _e) * rad, 1/real(3));
      ang /= 3;
      u = rad * cos(ang);
      v = rad * sin(ang) + _Ev.K();
    } else {
      // Use spherical TM, Lee 12.6 -- writing atanh(sin(lam) / cosh(psi)) =
      // asinh(sin(lam) / hypot(cos(lam), sinh(psi))).  This takes care of the
      // log singularity at zeta = Eu.K() (corresponding to the north pole)
      v = Math::asinh(sin(lam) / Math::hypot(cos(lam), sinh(psi)));
      u = atan2(sinh(psi), cos(lam));
      // But scale to put 90,0 on the right place
      u *= _Eu.K() / (Constants::pi()/2);
      v *= _Eu.K() / (Constants::pi()/2);
    }
    return retval;
  }

  // Invert zeta using Newton's method
  void TransverseMercatorExact::zetainv(real psi, real lam, real& u, real& v)
    const throw() {
    if (zetainv0(psi, lam, u, v))
      return;
    real stol2 = tol2 / sq(max(psi, real(1)));
    // min iterations = 2, max iterations = 6; mean = 4.0
    for (int i = 0, trip = 0; i < numit; ++i) {
      real snu, cnu, dnu, snv, cnv, dnv;
      _Eu.sncndn(u, snu, cnu, dnu);
      _Ev.sncndn(v, snv, cnv, dnv);
      real psi1, lam1, du1, dv1;
      zeta(u, snu, cnu, dnu, v, snv, cnv, dnv, psi1, lam1);
      dwdzeta(u, snu, cnu, dnu, v, snv, cnv, dnv, du1, dv1);
      psi1 -= psi;
      lam1 -= lam;
      real
        delu = psi1 * du1 - lam1 * dv1,
        delv = psi1 * dv1 + lam1 * du1;
      u -= delu;
      v -= delv;
      if (trip)
        break;
      real delw2 = sq(delu) + sq(delv);
      if (delw2 < stol2)
        ++trip;
    }
  }

  void TransverseMercatorExact::sigma(real u, real snu, real cnu, real dnu,
                                      real v, real snv, real cnv, real dnv,
                                      real& xi, real& eta) const throw() {
    // Lee 55.4 writing
    // dnu^2 + dnv^2 - 1 = _mu * cnu^2 + _mv * cnv^2
    real d = _mu * sq(cnu) + _mv * sq(cnv);
    xi = _Eu.E(snu, cnu, dnu) - _mu * snu * cnu * dnu / d;
    eta = v - _Ev.E(snv, cnv, dnv) + _mv * snv * cnv * dnv / d;
  }

  void TransverseMercatorExact::dwdsigma(real u, real snu, real cnu, real dnu,
                                         real v, real snv, real cnv, real dnv,
                                         real& du, real& dv) const throw() {
    // Reciprocal of 55.9: dw/ds = dn(w)^2/_mv, expanding complex dn(w) using
    // A+S 16.21.4
    real d = _mv * sq(sq(cnv) + _mu * sq(snu * snv));
    real
      dnr = dnu * cnv * dnv,
      dni = - _mu * snu * cnu * snv;
    du = (sq(dnr) - sq(dni)) / d;
    dv = 2 * dnr * dni / d;
  }

  // Starting point for sigmainv
  bool TransverseMercatorExact::sigmainv0(real xi, real eta, real& u, real& v)
    const throw() {
    bool retval = false;
    if (eta > real(1.25) * _Ev.KE() ||
        (xi < -real(0.25) * _Eu.E() && xi < eta - _Ev.KE())) {
      // sigma as a simple pole at w = w0 = Eu.K() + i * Ev.K() and sigma is
      // approximated by
      //
      // sigma = (Eu.E() + i * Ev.KE()) + 1/(w - w0)
      real
        x = xi - _Eu.E(),
        y = eta - _Ev.KE(),
        r2 = sq(x) + sq(y);
      u = _Eu.K() + x/r2;
      v = _Ev.K() - y/r2;
    } else if ((eta > real(0.75) * _Ev.KE() && xi < real(0.25) * _Eu.E())
               || eta > _Ev.KE()) {
      // At w = w0 = i * Ev.K(), we have
      //
      //     sigma = sigma0 = i * Ev.KE()
      //     sigma' = sigma'' = 0
      //
      // including the next term in the Taylor series gives:
      //
      // sigma = sigma0 - _mv / 3 * (w - w0)^3
      //
      // When inverting this, we map arg(w - w0) = [-pi/2, -pi/6] to
      // arg(sigma - sigma0) = [-pi/2, pi/2]
      // mapping arg = [-pi/2, -pi/6] to [-pi/2, pi/2]
      real
        deta = eta - _Ev.KE(),
        rad = Math::hypot(xi, deta),
        // Map the range [-90, 180] in sigma space to [-90, 0] in w space.  See
        // discussion in zetainv0 on the cut for ang.
        ang = atan2(deta-xi, xi+deta) - real(0.75) * Constants::pi();
      // Error using this guess is about 0.068 * rad^(5/3)
      retval = rad < 2 * taytol;
      rad = pow(3 / _mv * rad, 1/real(3));
      ang /= 3;
      u = rad * cos(ang);
      v = rad * sin(ang) + _Ev.K();
    } else {
      // Else use w = sigma * Eu.K/Eu.E (which is correct in the limit _e -> 0)
      u = xi * _Eu.K()/_Eu.E();
      v = eta * _Eu.K()/_Eu.E();
    }
    return retval;
  }

  // Invert sigma using Newton's method
  void TransverseMercatorExact::sigmainv(real xi, real eta, real& u, real& v)
    const throw() {
    if (sigmainv0(xi, eta, u, v))
      return;
    // min iterations = 2, max iterations = 7; mean = 3.9
    for (int i = 0, trip = 0; i < numit; ++i) {
      real snu, cnu, dnu, snv, cnv, dnv;
      _Eu.sncndn(u, snu, cnu, dnu);
      _Ev.sncndn(v, snv, cnv, dnv);
      real xi1, eta1, du1, dv1;
      sigma(u, snu, cnu, dnu, v, snv, cnv, dnv, xi1, eta1);
      dwdsigma(u, snu, cnu, dnu, v, snv, cnv, dnv, du1, dv1);
      xi1 -= xi;
      eta1 -= eta;
      real
        delu = xi1 * du1 - eta1 * dv1,
        delv = xi1 * dv1 + eta1 * du1;
      u -= delu;
      v -= delv;
      if (trip)
        break;
      real delw2 = sq(delu) + sq(delv);
      if (delw2 < tol2)
        ++trip;
    }
  }

  void TransverseMercatorExact::Scale(real phi, real lam,
                                      real snu, real cnu, real dnu,
                                      real snv, real cnv, real dnv,
                                      real& gamma, real& k) const throw() {
    real
      c = cos(phi),
      s = sin(lam),
      c2 = sq(c),
      s2 = sq(s),
      d = 1 - s2 * c2;
    // See comment after else clause for a discussion.
    if ( !( phi > 0 && d > real(0.75) && c2 * sq(c2 * _ep2) * s2 < tol ) ) {
      // Lee 55.12 -- negated for our sign convention.  gamma gives the bearing
      // (clockwise from true north) of grid north
      gamma = atan2(_mv * snu * snv * cnv, cnu * dnu * dnv);
      // Lee 55.13 with nu given by Lee 9.1 -- in sqrt change the numerator
      // from
      //
      //    (1 - snu^2 * dnv^2) to (_mv * snv^2 + cnu^2 * dnv^2)
      //
      // to maintain accuracy near phi = 90 and change the denomintor from
      //
      //    (dnu^2 + dnv^2 - 1) to (_mu * cnu^2 + _mv * cnv^2)
      //
      // to maintain accuracy near phi = 0, lam = 90 * (1 - e).  Similarly
      // rewrite sqrt term in 9.1 as
      //
      //    _mv + _mu * c^2 instead of 1 - _mu * sin(phi)^2
      k = sqrt(_mv + _mu * sq(c)) / c *
        sqrt( (_mv * sq(snv) + sq(cnu * dnv)) /
              (_mu * sq(cnu) + _mv * sq(cnv)) );
    } else {           //  phi > 0 && d > 0.75 && c2 * sq(c2 * _ep2) * s2 < tol
      // Use series approximations for gamma and k accurate to ep2.  Ignored
      // O(ep2^2) are shown here as gam2 and k2.  So only use these series if
      // the larger O(ep2^2) term (gam2) is less that tol.
      real
        // gam2 = c2^3*s2*((4*c2^2-c2)*s2^2+(8-9*c2)*s2-2)/(3*d^3)*ep2^2
        gam1 = sq(c2) * s2 / d * _ep2,
        // k2 = c2^3*s2^2*((3*c2^3+12*c2^2-28*c2)*s2^2+
        // (-34*c2^2+124*c2-64)*s2-61*c2+48)/(24*d^4)*ep2^2
        k1 = gam1 * ((c2 - 2) * s2 + 1) / (2 * d);
      gamma = atan2(sin(phi) * s * (1 + gam1), cos(lam));
      k = (1 + k1) / sqrt(d);
    }
  }

  void TransverseMercatorExact::Forward(real lon0, real lat, real lon,
                                        real& x, real& y, real& gamma, real& k)
    const throw() {
    // Avoid losing a bit of accuracy in lon (assuming lon0 is an integer)
    if (lon - lon0 > 180)
      lon -= lon0 - 360;
    else if (lon - lon0 <= -180)
      lon -= lon0 + 360;
    else
      lon -= lon0;
    // Now lon in (-180, 180]
    // Explicitly enforce the parity
    int
      latsign = !_extendp && lat < 0 ? -1 : 1,
      lonsign = !_extendp && lon < 0 ? -1 : 1;
    lon *= lonsign;
    lat *= latsign;
    bool backside = !_extendp && lon > 90;
    if (backside) {
      if (lat == 0)
        latsign = -1;
      lon = 180 - lon;
    }
    real
      phi = lat * Constants::degree(),
      lam = lon * Constants::degree();

    // u,v = coordinates for the Thompson TM, Lee 54
    real u, v;
    if (lat == 90) {
      u = _Eu.K();
      v = 0;
    } else if (lat == 0 && lon == 90 * (1 - _e)) {
      u = 0;
      v = _Ev.K();
    } else
      zetainv(psi(phi), lam, u, v);

    real snu, cnu, dnu, snv, cnv, dnv;
    _Eu.sncndn(u, snu, cnu, dnu);
    _Ev.sncndn(v, snv, cnv, dnv);

    real xi, eta;
    sigma(u, snu, cnu, dnu, v, snv, cnv, dnv, xi, eta);
    if (backside)
      xi = 2 * _Eu.E() - xi;
    y = xi * _a * _k0 * latsign;
    x = eta * _a * _k0 * lonsign;

    Scale(phi, lam, snu, cnu, dnu, snv, cnv, dnv, gamma, k);
    gamma /= Constants::degree();
    if (backside)
      gamma = 180 - gamma;
    gamma *= latsign * lonsign;
    k *= _k0;
  }

  void TransverseMercatorExact::Reverse(real lon0, real x, real y,
                                        real& lat, real& lon,
                                        real& gamma, real& k)
    const throw() {
    // This undoes the steps in Forward.
    real
      xi = y / (_a * _k0),
      eta = x / (_a * _k0);
    // Explicitly enforce the parity
    int
      latsign = !_extendp && y < 0 ? -1 : 1,
      lonsign = !_extendp && x < 0 ? -1 : 1;
    xi *= latsign;
    eta *= lonsign;
    bool backside = !_extendp && xi > _Eu.E();
    if (backside)
      xi = 2 * _Eu.E()- xi;

    // u,v = coordinates for the Thompson TM, Lee 54
    real u, v;
    if (xi == 0 && eta == _Ev.KE()) {
      u = 0;
      v = _Ev.K();
    } else
      sigmainv(xi, eta, u, v);

    real snu, cnu, dnu, snv, cnv, dnv;
    _Eu.sncndn(u, snu, cnu, dnu);
    _Ev.sncndn(v, snv, cnv, dnv);
    real phi, lam;
    if (v != 0 || u != _Eu.K()) {
      real psi;
      zeta(u, snu, cnu, dnu, v, snv, cnv, dnv, psi, lam);
      phi = psiinv(psi);
      lat = phi / Constants::degree();
      lon = lam / Constants::degree();
    } else {
      phi = Constants::pi()/2;
      lat = 90;
      lon = lam = 0;
    }
    Scale(phi, lam, snu, cnu, dnu, snv, cnv, dnv, gamma, k);
    gamma /= Constants::degree();
    if (backside)
      lon = 180 - lon;
    lon *= lonsign;
    // Avoid losing a bit of accuracy in lon (assuming lon0 is an integer)
    if (lon + lon0 >= 180)
      lon += lon0 - 360;
    else if (lon + lon0 < -180)
      lon += lon0 + 360;
    else
      lon += lon0;
    lat *= latsign;
    if (backside)
      y = 2 * _Eu.E() - y;
    y *= _a * _k0 * latsign;
    x *= _a * _k0 * lonsign;
    if (backside)
      gamma = 180 - gamma;
    gamma *= latsign * lonsign;
    k *= _k0;
  }

} // namespace GeographicLib
