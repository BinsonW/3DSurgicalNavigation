% ²úÉúÆåÅÌ¸ñ
clc
clear
close all

J = checkerboard(100,2,3);
K = J>0.5;

[h, w] = size(K);
n = 10;
I = ones(h+2*n, w+2*n);
I(n+1:n+h, n+1:n+w) = K;

figure
imshow(I)
impixelinfo

imwrite(I, 'checkerboard.png')

