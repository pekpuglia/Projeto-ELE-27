using ControlSystems, Plots
##
#W/mK
k_ar = 0.066
k_parede = 0.04
#W/m2K
h = 8
T_inf = 200
L_int = 2e-2
e = 1e-3
A_total = 9e-2*9e-2*4e-2
thermal_resistance = L_int^2/(2k_ar) + e/k_parede + 1/h
thermal_capacity = 1 #sei lá!
##
#entrada: T_inf

thermal_system = ss(-1/(thermal_resistance*thermal_capacity), 1/(thermal_resistance*thermal_capacity), 1, 0)
poles(thermal_system)