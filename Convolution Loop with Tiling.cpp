#include <thread>
#include <fstream>
#include <bits/stdc++.h>
#include <cstdlib>
using namespace std;

int input_im[1000][1000];
int filter_weight[3][3];
int filter_bias[3][3];
int output_im[1000][1000];
int tile_output[4][1000][1000];
int bias = 0;

void conv2d3x3(
    int pad, int stride, int input_size, int output_size, int row_start, int col_start, int tile_number)
{

    for (int i = 0; i < output_size; i++)
    {
        for (int j = 0; j < output_size; j++)
        {
            // compute one element in the output feature map
            float tmp = bias;

            for (int l = 0; l < 3; l++)
            {
                int h = row_start + (i * stride) + l - pad;

                for (int m = 0; m < 3; m++)
                {
                    int w = col_start + (j * stride) + m - pad;
                    if ((h >= 0) && (h < input_size) && (w >= 0) && (w < input_size))
                    {
                        tmp += input_im[h][w] * filter_weight[l][m];
                    }
                }
            }

            // add relu activation after conv
            tile_output[tile_number][i][j] = (tmp > 0) ? tmp : 0;
        }
    }
}

int main()
{
    string filename("input1.txt");
    ifstream input_file(filename);

    int input_size, stride, pad, tiles;
    input_file >> input_size;
    input_file >> stride;
    input_file >> pad;
    input_file >> tiles;
    int output_size = ((tiles)-3 + 2 * pad) / stride + 1;
    for (int i = 0; i < input_size; i++)
    {
        for (int j = 0; j < input_size; j++)
        {
            input_file >> input_im[i][j];
        }
    }
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            input_file >> filter_weight[i][j];
        }
    }
    vector<pair<int, int>> tile_index;
    for (int i = 0; i < input_size; i += tiles)
    {
        for (int j = 0; j < input_size; j += tiles)
        {
            tile_index.push_back({i, j});
        }
    }
    vector<thread> threads(tile_index.size());
    for (int i = 0; i < tile_index.size(); i++)
    {
        threads[i] = thread(conv2d3x3, pad, stride, input_size, output_size, tile_index[i].first, tile_index[i].second, i);
    }
    for (int i = 0; i < tile_index.size(); i++)
    {
        threads[i].join();
    }
    return 0;
}