procs = [1,4,9,16,25,36,49];
results_1680 = [43.852, 9.665, 7.966, 10.843, 10.607, 15.448,17.334];

speedup_1680 = results_1680(1) ./ results_1680;
    

results_840 = [3.792, 0.798,1.595,3.171 ,5.242,4.780 ,4.534];
speedup_840 = results_840(1) ./ results_840;
results_2100 = [96.001 29.281, 16.908 17.450,20.592,23.040,22.873 ];
speedup_2100 = results_2100(1) ./ results_2100;

plot(procs, speedup_840, procs, speedup_1680, procs, speedup_2100)
legend('840x840','1680x1680','2100x2100')