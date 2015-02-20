procs  = [1,4,9,16,25,36,49];
procs2 = [1,4,9,25,36,49];


results_210  = [0.039 0.019  0.024 0.030 0.040 0.046];
results_410  = [0.364 0.110 0.237 0.970 1.025 1.067  2.010];
speedup_210 = results_210(1) ./ results_210;
speedup_410 = results_410(1) ./ results_410;
results_630 = [1.530 0.319 0.874 3.437 3.976 3.061];
speedup_630 = results_630(1) ./ results_630;
results_1680 = [43.852, 9.665, 7.966, 10.843, 10.607, 15.448,17.334];
speedup_1680 = results_1680(1) ./ results_1680;
results_840  = [3.792, 0.798,1.595,3.171 ,5.242,4.780 ,4.534];
speedup_840  = results_840(1) ./ results_840;
results_2100 = [96.001 29.281, 16.908 17.450,20.592,23.040,22.873 ];
speedup_2100 = results_2100(1) ./ results_2100;

plot(procs2, speedup_210, procs, speedup_410, procs2, speedup_630, ... 
    procs, speedup_840, procs, speedup_1680, procs, speedup_2100, ...
    'LineWidth', 1.5)
set(gca, 'FontSize', 12)
legend('210x210','410x410', '630x630','840x840','1680x1680','2100x2100')
ylabel('Speedup compared to one processor')
xlabel('Number of processors')