% ----------------------------------------------------------------------
%   IMPLICIT INTEGRATION FOR CLOTH SIMULATION
%
% An implicit integrator and accompanying spring force 
% to simulate a particle system for clothing.
%
% Niels Joubert, Sebastian Burke
% UC Berkeley
% ----------------------------------------------------------------------

clear all;
clc;
figure(1);
clf();

% ----------------------------------------------------------------------
% DEFINE THE SIMULATION
% ----------------------------------------------------------------------

% We define the parameters of our simulation:
h = 0.02;        % seconds          - duration of a single timestep
ks = 100;        % force/meter      - spring constant
kd = 5;          % force*s/meter    - damping constant
dim = 3;         % dim              - 3d simulation
totaltime = 10;  % seconds          - Amount of time to simulate
g = 0.6;         % ratio            - ratio between explicit and implicit in Newmark.

% THREE DIFFERENT POSSIBLE GRIDS TO SIMULATE:
% Choose one of them by uncommenting...

%-----------------
%1 square example:
%-----------------
n = 5;
x = [-1.0 -1.0 0  0.1 -1.0 0  -1.0 0.1 0  0.1 0.1 0  -0.45 -0.45 0.0];
edges = [5 1; 1 2; 5 2; 2 4; 5 4; 4 3; 5 3; 3 1;];
triangles=[5 1 2;5 2 4;5 4 3;5 3 1;];
constNodes = [1];
%-----------------
%4 squares example:
%-----------------
%n = 13;
%x = [-1.0 -1.0 0.0  0.0 -1.0 0.0  1.0 -1.0 0.0  -1.0 0.0 0.0  0.0 0.0 0.0  1.0 0.0 0.0  -1.0 1.0 0.0  0.0 1.0 0.0  1.0 1.0 0.0  -0.5 -0.5 0.0  0.5 -0.5 0.0  -0.5 0.5 0.0  0.5 0.5 0.0];
%edges = [10 1 ; 1 2 ; 2 10 ; 2 5 ; 5 10 ; 5 4 ; 4 10 ; 4 1 ; 11 2 ; 2 3 ; 3 11 ; 3 6 ; 6 11 ; 6 5 ; 5 11 ; 12 4 ; 5 12 ; 5 8 ; 8 12 ; 8 7 ; 7 12 ; 7 4 ; 13 5 ; 6 13 ; 6 9 ; 9 13 ; 9 8 ; 8 13 ;];
%triangles = [10 1 2;10 2 5;10 5 4;10 4 1;11 2 3;11 3 6;11 6 5;11 5 2;12 4 5;12 5 8;12 8 7;12 7 4;13 5 6;13 6 9;13 9 8;13 8 5;];
%constNodes = [1];
%-----------------
%symmetric gid 0.20:
%-----------------
%n = 221;
%x = [-1.0 -1.0 0   -0.8 -1.0 0   -0.6 -1.0 0   -0.4 -1.0 0   -0.2 -1.0 0   0.0 -1.0 0   0.2 -1.0 0   0.4 -1.0 0   0.6 -1.0 0   0.8 -1.0 0   1.0 -1.0 0   -1.0 -0.8 0   -0.8 -0.8 0   -0.6 -0.8 0   -0.4 -0.8 0   -0.2 -0.8 0   0.0 -0.8 0   0.2 -0.8 0   0.4 -0.8 0   0.6 -0.8 0   0.8 -0.8 0   1.0 -0.8 0   -1.0 -0.6 0   -0.8 -0.6 0   -0.6 -0.6 0   -0.4 -0.6 0   -0.2 -0.6 0   0.0 -0.6 0   0.2 -0.6 0   0.4 -0.6 0   0.6 -0.6 0   0.8 -0.6 0   1.0 -0.6 0   -1.0 -0.4 0   -0.8 -0.4 0   -0.6 -0.4 0   -0.4 -0.4 0   -0.2 -0.4 0   0.0 -0.4 0   0.2 -0.4 0   0.4 -0.4 0   0.6 -0.4 0   0.8 -0.4 0   1.0 -0.4 0   -1.0 -0.2 0   -0.8 -0.2 0   -0.6 -0.2 0   -0.4 -0.2 0   -0.2 -0.2 0   0.0 -0.2 0   0.2 -0.2 0   0.4 -0.2 0   0.6 -0.2 0   0.8 -0.2 0   1.0 -0.2 0   -1.0 0.0 0   -0.8 0.0 0   -0.6 0.0 0   -0.4 0.0 0   -0.2 0.0 0   0.0 0.0 0   0.2 0.0 0   0.4 0.0 0   0.6 0.0 0   0.8 0.0 0   1.0 0.0 0   -1.0 0.2 0   -0.8 0.2 0   -0.6 0.2 0   -0.4 0.2 0   -0.2 0.2 0   0.0 0.2 0   0.2 0.2 0   0.4 0.2 0   0.6 0.2 0   0.8 0.2 0   1.0 0.2 0   -1.0 0.4 0   -0.8 0.4 0   -0.6 0.4 0   -0.4 0.4 0   -0.2 0.4 0   0.0 0.4 0   0.2 0.4 0   0.4 0.4 0   0.6 0.4 0   0.8 0.4 0   1.0 0.4 0   -1.0 0.6 0   -0.8 0.6 0   -0.6 0.6 0   -0.4 0.6 0   -0.2 0.6 0   0.0 0.6 0   0.2 0.6 0   0.4 0.6 0   0.6 0.6 0   0.8 0.6 0   1.0 0.6 0   -1.0 0.8 0   -0.8 0.8 0   -0.6 0.8 0   -0.4 0.8 0   -0.2 0.8 0   0.0 0.8 0   0.2 0.8 0   0.4 0.8 0   0.6 0.8 0   0.8 0.8 0   1.0 0.8 0   -1.0 1.0 0   -0.8 1.0 0   -0.6 1.0 0   -0.4 1.0 0   -0.2 1.0 0   0.0 1.0 0   0.2 1.0 0   0.4 1.0 0   0.6 1.0 0   0.8 1.0 0   1.0 1.0 0   -0.9 -0.9 0.0   -0.7 -0.9 0.0   -0.5 -0.9 0.0   -0.3 -0.9 0.0   -0.1 -0.9 0.0   0.1 -0.9 0.0   0.3 -0.9 0.0   0.5 -0.9 0.0   0.7 -0.9 0.0   0.9 -0.9 0.0   -0.9 -0.7 0.0   -0.7 -0.7 0.0   -0.5 -0.7 0.0   -0.3 -0.7 0.0   -0.1 -0.7 0.0   0.1 -0.7 0.0   0.3 -0.7 0.0   0.5 -0.7 0.0   0.7 -0.7 0.0   0.9 -0.7 0.0   -0.9 -0.5 0.0   -0.7 -0.5 0.0   -0.5 -0.5 0.0   -0.3 -0.5 0.0   -0.1 -0.5 0.0   0.1 -0.5 0.0   0.3 -0.5 0.0   0.5 -0.5 0.0   0.7 -0.5 0.0   0.9 -0.5 0.0   -0.9 -0.3 0.0   -0.7 -0.3 0.0   -0.5 -0.3 0.0   -0.3 -0.3 0.0   -0.1 -0.3 0.0   0.1 -0.3 0.0   0.3 -0.3 0.0   0.5 -0.3 0.0   0.7 -0.3 0.0   0.9 -0.3 0.0   -0.9 -0.1 0.0   -0.7 -0.1 0.0   -0.5 -0.1 0.0   -0.3 -0.1 0.0   -0.1 -0.1 0.0   0.1 -0.1 0.0   0.3 -0.1 0.0   0.5 -0.1 0.0   0.7 -0.1 0.0   0.9 -0.1 0.0   -0.9 0.1 0.0   -0.7 0.1 0.0   -0.5 0.1 0.0   -0.3 0.1 0.0   -0.1 0.1 0.0   0.1 0.1 0.0   0.3 0.1 0.0   0.5 0.1 0.0   0.7 0.1 0.0   0.9 0.1 0.0   -0.9 0.3 0.0   -0.7 0.3 0.0   -0.5 0.3 0.0   -0.3 0.3 0.0   -0.1 0.3 0.0   0.1 0.3 0.0   0.3 0.3 0.0   0.5 0.3 0.0   0.7 0.3 0.0   0.9 0.3 0.0   -0.9 0.5 0.0   -0.7 0.5 0.0   -0.5 0.5 0.0   -0.3 0.5 0.0   -0.1 0.5 0.0   0.1 0.5 0.0   0.3 0.5 0.0   0.5 0.5 0.0   0.7 0.5 0.0   0.9 0.5 0.0   -0.9 0.7 0.0   -0.7 0.7 0.0   -0.5 0.7 0.0   -0.3 0.7 0.0   -0.1 0.7 0.0   0.1 0.7 0.0   0.3 0.7 0.0   0.5 0.7 0.0   0.7 0.7 0.0   0.9 0.7 0.0   -0.9 0.9 0.0   -0.7 0.9 0.0   -0.5 0.9 0.0   -0.3 0.9 0.0   -0.1 0.9 0.0   0.1 0.9 0.0   0.3 0.9 0.0   0.5 0.9 0.0   0.7 0.9 0.0   0.9 0.9 0.0];   
%edges = [122 1 ; 1 2 ; 2 122 ; 2 13 ; 13 122 ; 13 12 ; 12 122 ; 12 1 ; 123 2 ; 2 3 ; 3 123 ; 3 14 ; 14 123 ; 14 13 ; 13 123 ; 124 3 ; 3 4 ; 4 124 ; 4 15 ; 15 124 ; 15 14 ; 14 124 ; 125 4 ; 4 5 ; 5 125 ; 5 16 ; 16 125 ; 16 15 ; 15 125 ; 126 5 ; 5 6 ; 6 126 ; 6 17 ; 17 126 ; 17 16 ; 16 126 ; 127 6 ; 6 7 ; 7 127 ; 7 18 ; 18 127 ; 18 17 ; 17 127 ; 128 7 ; 7 8 ; 8 128 ; 8 19 ; 19 128 ; 19 18 ; 18 128 ; 129 8 ; 8 9 ; 9 129 ; 9 20 ; 20 129 ; 20 19 ; 19 129 ; 130 9 ; 9 10 ; 10 130 ; 10 21 ; 21 130 ; 21 20 ; 20 130 ; 131 10 ; 10 11 ; 11 131 ; 11 22 ; 22 131 ; 22 21 ; 21 131 ; 132 12 ; 13 132 ; 13 24 ; 24 132 ; 24 23 ; 23 132 ; 23 12 ; 133 13 ; 14 133 ; 14 25 ; 25 133 ; 25 24 ; 24 133 ; 134 14 ; 15 134 ; 15 26 ; 26 134 ; 26 25 ; 25 134 ; 135 15 ; 16 135 ; 16 27 ; 27 135 ; 27 26 ; 26 135 ; 136 16 ; 17 136 ; 17 28 ; 28 136 ; 28 27 ; 27 136 ; 137 17 ; 18 137 ; 18 29 ; 29 137 ; 29 28 ; 28 137 ; 138 18 ; 19 138 ; 19 30 ; 30 138 ; 30 29 ; 29 138 ; 139 19 ; 20 139 ; 20 31 ; 31 139 ; 31 30 ; 30 139 ; 140 20 ; 21 140 ; 21 32 ; 32 140 ; 32 31 ; 31 140 ; 141 21 ; 22 141 ; 22 33 ; 33 141 ; 33 32 ; 32 141 ; 142 23 ; 24 142 ; 24 35 ; 35 142 ; 35 34 ; 34 142 ; 34 23 ; 143 24 ; 25 143 ; 25 36 ; 36 143 ; 36 35 ; 35 143 ; 144 25 ; 26 144 ; 26 37 ; 37 144 ; 37 36 ; 36 144 ; 145 26 ; 27 145 ; 27 38 ; 38 145 ; 38 37 ; 37 145 ; 146 27 ; 28 146 ; 28 39 ; 39 146 ; 39 38 ; 38 146 ; 147 28 ; 29 147 ; 29 40 ; 40 147 ; 40 39 ; 39 147 ; 148 29 ; 30 148 ; 30 41 ; 41 148 ; 41 40 ; 40 148 ; 149 30 ; 31 149 ; 31 42 ; 42 149 ; 42 41 ; 41 149 ; 150 31 ; 32 150 ; 32 43 ; 43 150 ; 43 42 ; 42 150 ; 151 32 ; 33 151 ; 33 44 ; 44 151 ; 44 43 ; 43 151 ; 152 34 ; 35 152 ; 35 46 ; 46 152 ; 46 45 ; 45 152 ; 45 34 ; 153 35 ; 36 153 ; 36 47 ; 47 153 ; 47 46 ; 46 153 ; 154 36 ; 37 154 ; 37 48 ; 48 154 ; 48 47 ; 47 154 ; 155 37 ; 38 155 ; 38 49 ; 49 155 ; 49 48 ; 48 155 ; 156 38 ; 39 156 ; 39 50 ; 50 156 ; 50 49 ; 49 156 ; 157 39 ; 40 157 ; 40 51 ; 51 157 ; 51 50 ; 50 157 ; 158 40 ; 41 158 ; 41 52 ; 52 158 ; 52 51 ; 51 158 ; 159 41 ; 42 159 ; 42 53 ; 53 159 ; 53 52 ; 52 159 ; 160 42 ; 43 160 ; 43 54 ; 54 160 ; 54 53 ; 53 160 ; 161 43 ; 44 161 ; 44 55 ; 55 161 ; 55 54 ; 54 161 ; 162 45 ; 46 162 ; 46 57 ; 57 162 ; 57 56 ; 56 162 ; 56 45 ; 163 46 ; 47 163 ; 47 58 ; 58 163 ; 58 57 ; 57 163 ; 164 47 ; 48 164 ; 48 59 ; 59 164 ; 59 58 ; 58 164 ; 165 48 ; 49 165 ; 49 60 ; 60 165 ; 60 59 ; 59 165 ; 166 49 ; 50 166 ; 50 61 ; 61 166 ; 61 60 ; 60 166 ; 167 50 ; 51 167 ; 51 62 ; 62 167 ; 62 61 ; 61 167 ; 168 51 ; 52 168 ; 52 63 ; 63 168 ; 63 62 ; 62 168 ; 169 52 ; 53 169 ; 53 64 ; 64 169 ; 64 63 ; 63 169 ; 170 53 ; 54 170 ; 54 65 ; 65 170 ; 65 64 ; 64 170 ; 171 54 ; 55 171 ; 55 66 ; 66 171 ; 66 65 ; 65 171 ; 172 56 ; 57 172 ; 57 68 ; 68 172 ; 68 67 ; 67 172 ; 67 56 ; 173 57 ; 58 173 ; 58 69 ; 69 173 ; 69 68 ; 68 173 ; 174 58 ; 59 174 ; 59 70 ; 70 174 ; 70 69 ; 69 174 ; 175 59 ; 60 175 ; 60 71 ; 71 175 ; 71 70 ; 70 175 ; 176 60 ; 61 176 ; 61 72 ; 72 176 ; 72 71 ; 71 176 ; 177 61 ; 62 177 ; 62 73 ; 73 177 ; 73 72 ; 72 177 ; 178 62 ; 63 178 ; 63 74 ; 74 178 ; 74 73 ; 73 178 ; 179 63 ; 64 179 ; 64 75 ; 75 179 ; 75 74 ; 74 179 ; 180 64 ; 65 180 ; 65 76 ; 76 180 ; 76 75 ; 75 180 ; 181 65 ; 66 181 ; 66 77 ; 77 181 ; 77 76 ; 76 181 ; 182 67 ; 68 182 ; 68 79 ; 79 182 ; 79 78 ; 78 182 ; 78 67 ; 183 68 ; 69 183 ; 69 80 ; 80 183 ; 80 79 ; 79 183 ; 184 69 ; 70 184 ; 70 81 ; 81 184 ; 81 80 ; 80 184 ; 185 70 ; 71 185 ; 71 82 ; 82 185 ; 82 81 ; 81 185 ; 186 71 ; 72 186 ; 72 83 ; 83 186 ; 83 82 ; 82 186 ; 187 72 ; 73 187 ; 73 84 ; 84 187 ; 84 83 ; 83 187 ; 188 73 ; 74 188 ; 74 85 ; 85 188 ; 85 84 ; 84 188 ; 189 74 ; 75 189 ; 75 86 ; 86 189 ; 86 85 ; 85 189 ; 190 75 ; 76 190 ; 76 87 ; 87 190 ; 87 86 ; 86 190 ; 191 76 ; 77 191 ; 77 88 ; 88 191 ; 88 87 ; 87 191 ; 192 78 ; 79 192 ; 79 90 ; 90 192 ; 90 89 ; 89 192 ; 89 78 ; 193 79 ; 80 193 ; 80 91 ; 91 193 ; 91 90 ; 90 193 ; 194 80 ; 81 194 ; 81 92 ; 92 194 ; 92 91 ; 91 194 ; 195 81 ; 82 195 ; 82 93 ; 93 195 ; 93 92 ; 92 195 ; 196 82 ; 83 196 ; 83 94 ; 94 196 ; 94 93 ; 93 196 ; 197 83 ; 84 197 ; 84 95 ; 95 197 ; 95 94 ; 94 197 ; 198 84 ; 85 198 ; 85 96 ; 96 198 ; 96 95 ; 95 198 ; 199 85 ; 86 199 ; 86 97 ; 97 199 ; 97 96 ; 96 199 ; 200 86 ; 87 200 ; 87 98 ; 98 200 ; 98 97 ; 97 200 ; 201 87 ; 88 201 ; 88 99 ; 99 201 ; 99 98 ; 98 201 ; 202 89 ; 90 202 ; 90 101 ; 101 202 ; 101 100 ; 100 202 ; 100 89 ; 203 90 ; 91 203 ; 91 102 ; 102 203 ; 102 101 ; 101 203 ; 204 91 ; 92 204 ; 92 103 ; 103 204 ; 103 102 ; 102 204 ; 205 92 ; 93 205 ; 93 104 ; 104 205 ; 104 103 ; 103 205 ; 206 93 ; 94 206 ; 94 105 ; 105 206 ; 105 104 ; 104 206 ; 207 94 ; 95 207 ; 95 106 ; 106 207 ; 106 105 ; 105 207 ; 208 95 ; 96 208 ; 96 107 ; 107 208 ; 107 106 ; 106 208 ; 209 96 ; 97 209 ; 97 108 ; 108 209 ; 108 107 ; 107 209 ; 210 97 ; 98 210 ; 98 109 ; 109 210 ; 109 108 ; 108 210 ; 211 98 ; 99 211 ; 99 110 ; 110 211 ; 110 109 ; 109 211 ; 212 100 ; 101 212 ; 101 112 ; 112 212 ; 112 111 ; 111 212 ; 111 100 ; 213 101 ; 102 213 ; 102 113 ; 113 213 ; 113 112 ; 112 213 ; 214 102 ; 103 214 ; 103 114 ; 114 214 ; 114 113 ; 113 214 ; 215 103 ; 104 215 ; 104 115 ; 115 215 ; 115 114 ; 114 215 ; 216 104 ; 105 216 ; 105 116 ; 116 216 ; 116 115 ; 115 216 ; 217 105 ; 106 217 ; 106 117 ; 117 217 ; 117 116 ; 116 217 ; 218 106 ; 107 218 ; 107 118 ; 118 218 ; 118 117 ; 117 218 ; 219 107 ; 108 219 ; 108 119 ; 119 219 ; 119 118 ; 118 219 ; 220 108 ; 109 220 ; 109 120 ; 120 220 ; 120 119 ; 119 220 ; 221 109 ; 110 221 ; 110 121 ; 121 221 ; 121 120 ; 120 221 ;];
%triangles=[122 1 2;122 2 13;122 13 12;122 12 1;123 2 3;123 3 14;123 14 13;123 13 2;124 3 4;124 4 15;124 15 14;124 14 3;125 4 5;125 5 16;125 16 15;125 15 4;126 5 6;126 6 17;126 17 16;126 16 5;127 6 7;127 7 18;127 18 17;127 17 6;128 7 8;128 8 19;128 19 18;128 18 7;129 8 9;129 9 20;129 20 19;129 19 8;130 9 10;130 10 21;130 21 20;130 20 9;131 10 11;131 11 22;131 22 21;131 21 10;132 12 13;132 13 24;132 24 23;132 23 12;133 13 14;133 14 25;133 25 24;133 24 13;134 14 15;134 15 26;134 26 25;134 25 14;135 15 16;135 16 27;135 27 26;135 26 15;136 16 17;136 17 28;136 28 27;136 27 16;137 17 18;137 18 29;137 29 28;137 28 17;138 18 19;138 19 30;138 30 29;138 29 18;139 19 20;139 20 31;139 31 30;139 30 19;140 20 21;140 21 32;140 32 31;140 31 20;141 21 22;141 22 33;141 33 32;141 32 21;142 23 24;142 24 35;142 35 34;142 34 23;143 24 25;143 25 36;143 36 35;143 35 24;144 25 26;144 26 37;144 37 36;144 36 25;145 26 27;145 27 38;145 38 37;145 37 26;146 27 28;146 28 39;146 39 38;146 38 27;147 28 29;147 29 40;147 40 39;147 39 28;148 29 30;148 30 41;148 41 40;148 40 29;149 30 31;149 31 42;149 42 41;149 41 30;150 31 32;150 32 43;150 43 42;150 42 31;151 32 33;151 33 44;151 44 43;151 43 32;152 34 35;152 35 46;152 46 45;152 45 34;153 35 36;153 36 47;153 47 46;153 46 35;154 36 37;154 37 48;154 48 47;154 47 36;155 37 38;155 38 49;155 49 48;155 48 37;156 38 39;156 39 50;156 50 49;156 49 38;157 39 40;157 40 51;157 51 50;157 50 39;158 40 41;158 41 52;158 52 51;158 51 40;159 41 42;159 42 53;159 53 52;159 52 41;160 42 43;160 43 54;160 54 53;160 53 42;161 43 44;161 44 55;161 55 54;161 54 43;162 45 46;162 46 57;162 57 56;162 56 45;163 46 47;163 47 58;163 58 57;163 57 46;164 47 48;164 48 59;164 59 58;164 58 47;165 48 49;165 49 60;165 60 59;165 59 48;166 49 50;166 50 61;166 61 60;166 60 49;167 50 51;167 51 62;167 62 61;167 61 50;168 51 52;168 52 63;168 63 62;168 62 51;169 52 53;169 53 64;169 64 63;169 63 52;170 53 54;170 54 65;170 65 64;170 64 53;171 54 55;171 55 66;171 66 65;171 65 54;172 56 57;172 57 68;172 68 67;172 67 56;173 57 58;173 58 69;173 69 68;173 68 57;174 58 59;174 59 70;174 70 69;174 69 58;175 59 60;175 60 71;175 71 70;175 70 59;176 60 61;176 61 72;176 72 71;176 71 60;177 61 62;177 62 73;177 73 72;177 72 61;178 62 63;178 63 74;178 74 73;178 73 62;179 63 64;179 64 75;179 75 74;179 74 63;180 64 65;180 65 76;180 76 75;180 75 64;181 65 66;181 66 77;181 77 76;181 76 65;182 67 68;182 68 79;182 79 78;182 78 67;183 68 69;183 69 80;183 80 79;183 79 68;184 69 70;184 70 81;184 81 80;184 80 69;185 70 71;185 71 82;185 82 81;185 81 70;186 71 72;186 72 83;186 83 82;186 82 71;187 72 73;187 73 84;187 84 83;187 83 72;188 73 74;188 74 85;188 85 84;188 84 73;189 74 75;189 75 86;189 86 85;189 85 74;190 75 76;190 76 87;190 87 86;190 86 75;191 76 77;191 77 88;191 88 87;191 87 76;192 78 79;192 79 90;192 90 89;192 89 78;193 79 80;193 80 91;193 91 90;193 90 79;194 80 81;194 81 92;194 92 91;194 91 80;195 81 82;195 82 93;195 93 92;195 92 81;196 82 83;196 83 94;196 94 93;196 93 82;197 83 84;197 84 95;197 95 94;197 94 83;198 84 85;198 85 96;198 96 95;198 95 84;199 85 86;199 86 97;199 97 96;199 96 85;200 86 87;200 87 98;200 98 97;200 97 86;201 87 88;201 88 99;201 99 98;201 98 87;202 89 90;202 90 101;202 101 100;202 100 89;203 90 91;203 91 102;203 102 101;203 101 90;204 91 92;204 92 103;204 103 102;204 102 91;205 92 93;205 93 104;205 104 103;205 103 92;206 93 94;206 94 105;206 105 104;206 104 93;207 94 95;207 95 106;207 106 105;207 105 94;208 95 96;208 96 107;208 107 106;208 106 95;209 96 97;209 97 108;209 108 107;209 107 96;210 97 98;210 98 109;210 109 108;210 108 97;211 98 99;211 99 110;211 110 109;211 109 98;212 100 101;212 101 112;212 112 111;212 111 100;213 101 102;213 102 113;213 113 112;213 112 101;214 102 103;214 103 114;214 114 113;214 113 102;215 103 104;215 104 115;215 115 114;215 114 103;216 104 105;216 105 116;216 116 115;216 115 104;217 105 106;217 106 117;217 117 116;217 116 105;218 106 107;218 107 118;218 118 117;218 117 106;219 107 108;219 108 119;219 119 118;219 118 107;220 108 109;220 109 120;220 120 119;220 119 108;221 109 110;221 110 121;221 121 120;221 120 109;];
%constNodes = [21 121];

% SIMULATION INITIAL PARAMETERS
v = zeros(1,n*dim);                 %velocity starts at 0.
M = (0.1 / n)*eye(n*dim, n*dim);      %mass per point
Minv = inv(M);

% CALCULATE REST LENGTHS
for i=1:length(edges(:,1)),
    ia = (edges(i,1) - 1)*3 + 1;
    ib = (edges(i,2) - 1)*3 + 1;
    xa = x(ia:ia+2);
    xb = x(ib:ib+2);
    rli = norm(xb-xa);
    rl(i) = rli;   
end

% CALCULATE CONSTRAINTED PARTICLES


const = [(constNodes-1)*3+1 (constNodes-1)*3+2 (constNodes-1)*3+3;];
globalToSol = ones(dim*n, 1);
globalToSol(const) = 0;
globalToSol = max(1, cumsum(globalToSol));

% ----------------------------------------------------------------------
% RUN THE SIMULATION
% ----------------------------------------------------------------------

steps = totaltime / h;
for j=1:steps,

    %draw:
    figure(1),clf, hold on
    for i=1:length(triangles(:,1)),
        paintx = [x((triangles(i,1)-1)*3+1), x((triangles(i,2)-1)*3+1), x((triangles(i,3)-1)*3+1)];
        painty = [x((triangles(i,1)-1)*3+2), x((triangles(i,2)-1)*3+2), x((triangles(i,3)-1)*3+2)];
        paintz = [x((triangles(i,1)-1)*3+3), x((triangles(i,2)-1)*3+3), x((triangles(i,3)-1)*3+3)];
        patch(paintx, painty, paintz, 'r');
    end
    view(3);
    axis([-2 2 -2 2 -2 2]);
    hold off;
    
    %set up forces and jacobians:
    f = zeros(1,n*dim);
    fext = zeros(1,n*dim);
    JP = zeros(n*dim,n*dim);    %df/dx
    JV = zeros(n*dim,n*dim);    %df/dv
    
    %look at each triangle...
    for i=1:length(edges(:,1)),
        %Get values for edges
       ia = (edges(i,1) - 1)*3 + 1;
       ib = (edges(i,2) - 1)*3 + 1;
       xa = x(ia:ia+2);
       xb = x(ib:ib+2);
       va = v(ia:ia+2);
       vb = v(ib:ib+2);
       
       %Calculate total force on each particle due to this triangle
       fa = fsa(xa,xb,rl(i),ks) + fda(xa,xb,va,vb,kd);
       fb = fsa(xb,xa,rl(i),ks) + fda(xb,xa,vb,va,kd);
       %Accumulate Forces
       f(ia:ia+2) = f(ia:ia+2) + fa;
       f(ib:ib+2) = f(ib:ib+2) + fb;
       
       %Calculate total jacobian wrt position and velocity for each
       %connection (each edge in the grid).
       JP_fa_xa = jdap(xa,xb,va,vb,rl(i),kd) + jsap(xa, xb, rl(i), ks);
       JP_fb_xa = -1*JP_fa_xa;
       JP_fb_xb = JP_fa_xa;
       JP_fa_xb = -1*JP_fb_xb;
       
       JV_fa_xa = jdav(xa, xb, va, vb, rl(i), kd);
       JV_fb_xa = -1*JV_fa_xa;
       JV_fb_xb = JV_fa_xa;
       JV_fa_xb = -1*JV_fb_xb;
       
       %Accumulate Jacobians
       JP(ia:ia+2,ia:ia+2) = JP(ia:ia+2,ia:ia+2) + JP_fa_xa;
       JP(ia:ia+2,ib:ib+2) = JP(ia:ia+2,ib:ib+2) + JP_fa_xb;
       JP(ib:ib+2,ia:ia+2) = JP(ib:ib+2,ia:ia+2) + JP_fb_xa;
       JP(ib:ib+2,ib:ib+2) = JP(ib:ib+2,ib:ib+2) + JP_fb_xb;
       JV(ia:ia+2,ia:ia+2) = JV(ia:ia+2,ia:ia+2) + JV_fa_xa;
       JV(ia:ia+2,ib:ib+2) = JV(ia:ia+2,ib:ib+2) + JV_fa_xb;
       JV(ib:ib+2,ia:ia+2) = JV(ib:ib+2,ia:ia+2) + JV_fb_xa;
       JV(ib:ib+2,ib:ib+2) = JV(ib:ib+2,ib:ib+2) + JV_fb_xb;
    end
    
    for i=1:3:3*n,
       fext(i:i+2) = + M(i,i)*[0 0 -9.8]; %gravity
    end
    
    fext(const) = 0; %constraints
    f = f + fext;
    
    %%% We now need to solve the Ax = b system,
    %%% where x is change in velocity.
    
    %=== EXPLICIT FORWARD EULER: ===
    %delX = h*v;
    %delV = h*f*inv(M);
    
    %=== IMPLICIT BACKWARDS EULER: ===
    
    %These are the defining equations. These are not symmetric
    %if particles do not all have the same mass.
    %A = eye(3*n) - h*Minv*JV - h^2*Minv*JP;
    %b = h*(f + h*v*JP)*Minv;
    
    %These equations give us a symmetric, positive definite system.
    %Unfortunately this only works if you don't do mass modification
    %to enforce constraints.
    %A = M - h*JV - h^2*JP;
    %b = h*(f + h*v*JP);
    
    %delV = b/A;
    %delX = h*(v + delV);
    
    %=== NEWMARK INTEGRATOR ===
    A = M - g*h*JV - g*h^2*JP;
    b = h*(f + g*h*v*JP);
    
    %Constraints
    A(const, :) = [];
    A(:, const) = [];
    b(const) = [];
    
    %Solve for change in V
    delV = b/A;
    
    %take constraints into account
    delV = delV(globalToSol);
    delV(const) = 0.0;
    
    %calculate change in positon
    delX = h*(v + g*delV);   %This is nuttapong's... correct?
    %delX = h*(v + delV);    %This is our original...
   
    %update the simulation state:
    x = x + delX;
    v = v + delV;
    
end