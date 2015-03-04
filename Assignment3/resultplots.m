
% Result from bucketsort 1.000 100.000.000
% for 8 16 32 threads
method1_1k = [2.0973 1.0721 0.6956];
method2_1k = [2.0771 1.0682 0.6911];
method3_1k = [2.0943 1.0920 0.7391];

% 1.000 threads all the time
method4_1k = 1.1225;

% Result from bucketsort 10.000 100.000.000
% for 8 16 32 threads
method1_10k = [1.7037 0.8651 0.5064];
method2_10k = [1.7020 0.8711 0.5882];
method3_10k = [1.6898 0.8537 0.4959];

% method 4 did not work for 10.000

threads = [8 16 32];

plot(threads, method1_1k, threads, method2_1k, threads, method3_1k)