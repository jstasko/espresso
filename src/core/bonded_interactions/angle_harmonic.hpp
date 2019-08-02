/*
  Copyright (C) 2010-2018 The ESPResSo project
  Copyright (C) 2002,2003,2004,2005,2006,2007,2008,2009,2010
    Max-Planck-Institute for Polymer Research, Theory Group

  This file is part of ESPResSo.

  ESPResSo is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ESPResSo is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef ANGLE_HARMONIC_H
#define ANGLE_HARMONIC_H
/** \file
 *  Routines to calculate the angle energy or/and and force
 *  for a particle triple using the potential described in
 *  @ref bondedIA_angle_harmonic.
 */

#include "bonded_interaction_data.hpp"
#include "particle_data.hpp"

#include "angle_common.hpp"
#include "grid.hpp"

#include <utils/math/sqr.hpp>

#include <tuple>

/** Set parameters for the angle potential. */
int angle_harmonic_set_params(int bond_type, double bend, double phi0);

/** Compute the three-body angle interaction force.
 *  @param  p_mid     Second/middle particle.
 *  @param  p_left    First/left particle.
 *  @param  p_right   Third/right particle.
 *  @param  iaparams  Bonded parameters for the angle interaction.
 *  @return Forces on the second, first and third particles, in that order.
 */
inline std::tuple<Utils::Vector3d, Utils::Vector3d, Utils::Vector3d>
calc_angle_harmonic_3body_forces(Particle const *const p_mid,
                                 Particle const *const p_left,
                                 Particle const *const p_right,
                                 Bonded_ia_parameters const *const iaparams) {

  auto forceFactor = [&iaparams](double const cos_phi) {
    auto const sin_phi = sqrt(1 - Utils::sqr(cos_phi));
    auto const phi = acos(cos_phi);
    auto const phi0 = iaparams->p.angle_harmonic.phi0;
    auto const k = iaparams->p.angle_harmonic.bend;
    return -k * (phi - phi0) / sin_phi;
  };

  return calc_angle_generic_force(p_mid->r.p, p_left->r.p, p_right->r.p,
                                  forceFactor, true);
}

/** Compute the three-body angle interaction force.
 *  @param[in]  p_mid     Second/middle particle.
 *  @param[in]  p_left    First/left particle.
 *  @param[in]  p_right   Third/right particle.
 *  @param[in]  iaparams  Bonded parameters for the angle interaction.
 *  @return the forces on the second, first and third particles.
 */
inline std::tuple<Utils::Vector3d, Utils::Vector3d, Utils::Vector3d>
calc_angle_harmonic_force(Particle const *const p_mid,
                          Particle const *const p_left,
                          Particle const *const p_right,
                          Bonded_ia_parameters const *const iaparams) {
  return calc_angle_harmonic_3body_forces(p_mid, p_left, p_right, iaparams);
}

/** Compute the three-body angle interaction energy.
 *  @param[in]  p_mid     Second/middle particle.
 *  @param[in]  p_left    First/left particle.
 *  @param[in]  p_right   Third/right particle.
 *  @param[in]  iaparams  Bonded parameters for the angle interaction.
 */
inline double
angle_harmonic_energy(Particle const *const p_mid, Particle const *const p_left,
                      Particle const *const p_right,
                      Bonded_ia_parameters const *const iaparams) {
  auto const vectors =
      calc_vectors_and_cosine(p_mid->r.p, p_left->r.p, p_right->r.p, true);
  auto const cos_phi = std::get<4>(vectors);
  auto const phi = acos(cos_phi);
  auto const phi0 = iaparams->p.angle_harmonic.phi0;
  auto const k = iaparams->p.angle_harmonic.bend;
  auto const energy = 0.5 * k * Utils::sqr(phi - phi0);
  return energy;
}

#endif /* ANGLE_HARMONIC_H */
