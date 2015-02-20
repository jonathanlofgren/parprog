threads = [1 2 4 8 16 32 64 128];

pear_time = [18770 12231 9123 9144 12924 21678 39899 78007] ./ 1000;
dac_time = [19.162 11.459 10.227 8.779 6.052 5.700 5.120 4.911];

pear_speedup = pear_time(1) ./ pear_time;
dac_speedup = dac_time(1) ./ dac_time;

plot(threads, pear_speedup, threads, dac_speedup, 'LineWidth', 1.5)
xlabel('Maximum number of threads')
ylabel('Speedup factor')
legend('Peer sort','Divide and conquer')
set(gca, 'FontSize', 12)