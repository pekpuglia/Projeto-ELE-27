using ControlSystems, Plots, ISAData
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
#entrada: T_inf; Q
thermal_system = ss(-1/(thermal_resistance*thermal_capacity), [1/(thermal_resistance*thermal_capacity) 1/thermal_capacity], 1, 0)
##
ki = 10
kp = 10
control_system = ss(0, 1, ki, kp)
##
controlled_system = feedback(control_system, thermal_system, 
    Y2 = :, Z1=[], Z2= :, Y1  = :, U2 = [2], W2 = [1])
##
input_signal(X, t) = [298;ISAdata(5*t)[3]]
simr = lsim(controlled_system, input_signal, 0:30:6000, x0 = [0;298])
##
plot(simr)