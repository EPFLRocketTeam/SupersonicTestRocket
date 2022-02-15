import numpy as np

a = 340  # m/s
air_density = 1.225  # kg/m^3

plenum_volume = 0.05  # cubic meter
plenum_start_pressure = 700000  # Pa
plenum_start_density = 700000 / 101325 * air_density
plenum_start_mass = plenum_volume * plenum_start_density

a_star = 7.6519e-4  # m^2

plenum_pressure = plenum_start_pressure
plenum_density = plenum_start_density
plenum_mass = plenum_start_mass

timestep = 0.01
time = 0

while plenum_pressure > 200000:
    rho_star = 0.63393814 * plenum_density
    m_dot_star = rho_star * a * 1 * a_star
    plenum_mass -= m_dot_star * timestep

    plenum_pressure = plenum_mass / plenum_start_mass * plenum_start_pressure
    plenum_density = plenum_mass / plenum_start_mass * plenum_start_density
    time += timestep

print(f"Wind tunnel lasted {time:.3f} seconds")