time = [
  1 3.23
  2 1.93
  4 0.98
  6 0.67
  8 0.62
  12 0.44
  16 0.37
  32 0.35
  40 0.22
  50 0.95
  59 0.79
  64 3.89
];

plot(time(:,1), time(1,2)./time(:,2))
xlabel('Number of processors')
ylabel('Speedup')