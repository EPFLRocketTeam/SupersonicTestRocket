function [rotatedValues] = rotate2D(originalValues, theta)
%ROTATE2D Rotates a vector of original values according to theta vector
%   Takes a vector of original values in a given orthogonal frame and
%   rotates its individual components by the values in the theta vector.
%   Uses fast element-wise dot products
%
%INPUTS:
%   originalValues (n x 2): A vector of values in a given orthogonal frame
%   theta (n x 1): A rotation vector by which to rotate each individual
%       value in the originalValues vector.
%
%OUTPUTS:
%   rotatedValues (n x 2): The rotated values of the originalValues vector


% create the rotation matrix for all elements in the vector
R = [cos(theta), -sin(theta), ...
     sin(theta), cos(theta)];

% give it the right shape
R = permute(reshape(R',2,2,[]),[2,1,3]);

% put the vector to rotate in a 3d array
originalValuesPaged = reshape(originalValues',2,1,[]);

% calculate the new rotated values
rotatedValues = pagemtimes(R,originalValuesPaged);

% get them back into a 2d vector
rotatedValues = reshape(rotatedValues,2,[])';

end

