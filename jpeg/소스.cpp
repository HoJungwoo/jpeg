#include "stdio.h"
#include "math.h"

#pragma warning(disable:4996)

#define WIDTH 512
#define HEIGHT 512
#define PI 3.1415926535897



//double DCT_sigma(unsigned char data[][512], int p, int k, int x, int y);
void zig_move_right(int* zig_i, int* zig_j, int* flag, int* temp);
void zig_move_down(int* zig_i, int* zig_j, int* flag, int* temp);
void zig_move_diag_down(int* zig_i, int* zig_j, int* flag, int* temp);
void zig_move_diag_up(int* zig_i, int* zig_j, int* flag, int* temp);
//double Inv_DCT_sigma(unsigned char data[][512], int p, int k, int x, int y);
unsigned char DCT_ch[512][512] = { 0 };

//encoding
unsigned char input_data[WIDTH][HEIGHT];
double DCT_matrix[WIDTH][HEIGHT];
int quantizaition_matrix[WIDTH][HEIGHT];
unsigned char qp_table[8][8] = {
   {16,11,10,16,24,40,51,61},
   {12,12,14,19,26,58,60,55},
   {14,13,16,24,40,57,69,56},
   {14,17,22,29,51,87,80,62},
   {18,22,37,56,68,109,103,77},
   {24,35,55,64,81,104,113,92},
   {49,64,78,87,103,121,120,101},
   {72,92,95,98,112,100,103,99}
};
int zigzag_matrix[64][64][64];
//decoding
int zigzag_matrix_inv[WIDTH][HEIGHT];
double quantizaition_matrix_inv[WIDTH][HEIGHT];
unsigned char DCT_matrix_inv[WIDTH][HEIGHT];



void main()
{

    FILE* input_file;
    input_file = fopen("lena.raw", "rb");
    if (input_file == NULL)
    {
        printf("File not found!!\n");
        return;
    }

    fread(input_data, sizeof(unsigned char), WIDTH * HEIGHT, input_file);







    //test[248~255][184~191]
    printf("<input data>\n");
    for (int i = 248; i < 256; i++)
    {
        for (int j = 184; j < 192; j++)
        {
            printf("%d\t", input_data[i][j]);
        }
        printf("\n");
    }
    printf("\n");




    //DCT  // space -> freq
    double DCT_C_H;
    double DCT_C_W;
    double result;


    for (int p = 0; p < 64; p++)
    {
        for (int k = 0; k < 64; k++)
        {
            for (int i = 0; i < 8; i++)
            {
                if (i == 0) { DCT_C_H = (double)1 / sqrt(2); }
                else { DCT_C_H = 1; }


                for (int j = 0; j < 8; j++)
                {
                    if (j == 0) { DCT_C_W = (double)1 / sqrt(2); }
                    else { DCT_C_W = 1; }
                    result = 0;
                    for (int m = 0; m < 8; m++)
                    {
                        for (int n = 0; n < 8; n++)
                        {
                            result += cos((((2.0 * (double)m + 1.0) * (double)i * PI) / 16.0)) * cos((((2.0 * (double)n + 1.0) * (double)j * PI) / 16.0)) * (double)input_data[(p * 8) + m][(k * 8) + n];
                        }
                    }
                    DCT_matrix[(p * 8) + i][(k * 8) + j] = result * DCT_C_H * DCT_C_W / 4.0;

                }
            }
        }
    }

    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            DCT_ch[i][j] = DCT_matrix[i][j];
        }
    }
    //printf("%f", DCT_sigma(input_data, 0, 0, 0, 0));



    //for (int i = 0; i < 8; i++)
    //{
    //    for (int j = 0; j < 8; j++)
    //    {
    //        printf("%10lf", DCT_matrix[i][j]);
    //    }
    //    printf("\n");
    //}






    //test[248~255][184~191]
    printf("<DCT data>\n");
    for (int i = 248; i < 256; i++)
    {
        for (int j = 184; j < 192; j++)
        {
            printf("%f\t", DCT_matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    //quantizaition
    for (int p = 0; p < 64; p++)
    {
        for (int k = 0; k < 64; k++)
        {
            for (int i = 0; i < 8; i++)
            {

                for (int j = 0; j < 8; j++)
                {
                    quantizaition_matrix[(p * 8) + i][(k * 8) + j] = (int) ((round)((float)DCT_matrix[(p * 8) + i][(k * 8) + j] / qp_table[i][j]));

                }
            }
        }
    }

    //test[248~255][184~191]
    printf("<quantizaition data>\n");
    for (int i = 248; i < 256; i++)
    {
        for (int j = 184; j < 192; j++)
        {
            printf("%d\t", quantizaition_matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");


    //zigzag
    int zig_i;
    int zig_j;
    int flag;
    int temp;


    for (int p = 0; p < 64; p++)
    {
        for (int k = 0; k < 64; k++)
        {
            zig_i = 0;
            zig_j = 0;
            flag = 1;
            temp = 0;
            for (int cnt = 0; cnt < 64; cnt++)
            {
                zigzag_matrix[p][k][cnt] = quantizaition_matrix[(p * 8) + zig_i][(k * 8) + zig_j]; // (0,0) start
                if (temp < 3)
                {
                    if (flag == 1) zig_move_right(&zig_i, &zig_j, &flag, &temp);
                    else if (flag == 2) zig_move_diag_down(&zig_i, &zig_j, &flag, &temp);
                    else if (flag == 3) zig_move_down(&zig_i, &zig_j, &flag, &temp);
                    else if (flag == 4) zig_move_diag_up(&zig_i, &zig_j, &flag, &temp);
                }
                else if (temp == 3)
                {
                    if (flag == 1) zig_move_right(&zig_i, &zig_j, &flag, &temp);
                    else if (flag == 2) zig_move_diag_down(&zig_i, &zig_j, &flag, &temp);
                    else if (flag == 3)
                    {
                        zig_move_right(&zig_i, &zig_j, &flag, &temp);
                        flag = 4;
                    }
                    else if (flag == 4) zig_move_diag_up(&zig_i, &zig_j, &flag, &temp);
                }
                else
                {
                    if (flag == 1) zig_move_down(&zig_i, &zig_j, &flag, &temp);
                    else if (flag == 2) zig_move_diag_down(&zig_i, &zig_j, &flag, &temp);
                    else if (flag == 3) zig_move_right(&zig_i, &zig_j, &flag, &temp);
                    else if (flag == 4) zig_move_diag_up(&zig_i, &zig_j, &flag, &temp);
                }
            }
        }
    }

    //test[248~255][184~191]
    printf("<zigzag data>\n");
	for (int i = 0; i < 64; i++)
	{

		printf("%d\t", zigzag_matrix[31][23][i]);

	}
    printf("\n\n");



    //inv_zigzag
    int inv_idx;
    int inv_flag;
    int inv_temp;


    for (int p = 0; p < 64; p++)
    {
        for (int k = 0; k < 64; k++)
        {
            inv_idx = 0;
            zig_i = 0;
            zig_j = 0;
            inv_temp = 0;
            inv_flag = 1;
            for (int i = 0; i < 8; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    zigzag_matrix_inv[(p * 8) + zig_i][(k * 8) + zig_j] = zigzag_matrix[p][k][inv_idx];

                    if (inv_temp < 3)
                    {
                        if (inv_flag == 1) zig_move_right(&zig_i, &zig_j, &inv_flag, &inv_temp);
                        else if (inv_flag == 2) zig_move_diag_down(&zig_i, &zig_j, &inv_flag, &inv_temp);
                        else if (inv_flag == 3) zig_move_down(&zig_i, &zig_j, &inv_flag, &inv_temp);
                        else if (inv_flag == 4) zig_move_diag_up(&zig_i, &zig_j, &inv_flag, &inv_temp);
                    }
                    else if (inv_temp == 3)
                    {
                        if (inv_flag == 1) zig_move_right(&zig_i, &zig_j, &inv_flag, &inv_temp);
                        else if (inv_flag == 2) zig_move_diag_down(&zig_i, &zig_j, &inv_flag, &inv_temp);
                        else if (inv_flag == 3)
                        {
                            zig_move_right(&zig_i, &zig_j, &inv_flag, &inv_temp);
                            inv_flag = 4;
                        }
                        else if (inv_flag == 4) zig_move_diag_up(&zig_i, &zig_j, &inv_flag, &inv_temp);
                    }
                    else
                    {
                        if (inv_flag == 1) zig_move_down(&zig_i, &zig_j, &inv_flag, &inv_temp);
                        else if (inv_flag == 2) zig_move_diag_down(&zig_i, &zig_j, &inv_flag, &inv_temp);
                        else if (inv_flag == 3) zig_move_right(&zig_i, &zig_j, &inv_flag, &inv_temp);
                        else if (inv_flag == 4) zig_move_diag_up(&zig_i, &zig_j, &inv_flag, &inv_temp);
                    }
                    inv_idx++;
                }
            }
        }
    }

    //test[248~255][184~191]
    printf("\n\n");
    printf("<inv_zigzag data>\n");
    for (int i = 248; i < 256; i++)
    {
        for (int j = 184; j < 192; j++)
        {
            printf("%d\t", zigzag_matrix_inv[i][j]);
        }
        printf("\n");
    }


    //inv_quantizaition

    for (int p = 0; p < 64; p++)
    {
        for (int k = 0; k < 64; k++)
        {
            for (int i = 0; i < 8; i++)
            {

                for (int j = 0; j < 8; j++)
                {

                    quantizaition_matrix_inv[(p * 8) + i][(k * 8) + j] = (zigzag_matrix_inv[(p * 8) + i][(k * 8) + j] * (double)qp_table[i][j]);

                }
            }
        }
    }

    //test[248~255][184~191]
    printf("\n\n");
    printf("<inv_quantizaition data>\n");
    for (int i = 248; i < 256; i++)
    {
        for (int j = 184; j < 192; j++)
        {
            printf("%f\t", quantizaition_matrix_inv[i][j]);
        }
        printf("\n");
    }

    //inv_DCT  //freq -> space
    for (int p = 0; p < 64; p++) {
        for (int k = 0; k < 64; k++)
        {
            for (int i = 0; i < 8; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    result = 0;
                    for (int m = 0; m < 8; m++)
                    {
                        if (m == 0) DCT_C_H = (double)1 / sqrt(2);
                        else DCT_C_H = 1;
                        for (int n = 0; n < 8; n++)
                        {
                            if (n == 0) DCT_C_W = (double)1 / sqrt(2);
                            else DCT_C_W = 1;
                            result += (double)quantizaition_matrix_inv[(8 * p) + m][(8 * k) + n] * DCT_C_H * DCT_C_W * cos(((2 * i + 1) * m * PI) / (double)16) * cos(((2 * j + 1) * n * PI) / (double)16);
                        }

                    }
                    DCT_matrix_inv[(p * 8) + i][(k * 8) + j] = (round)(result / 4.0);
                    //DCT_matrix_inv[(p * 8) + zig_i][(k * 8) + zig_j] = round(Inv_DCT_sigma(quantizaition_matrix_inv, p, k, i, j));
                }
            }
        }
    }

    //test[248~255][184~191]
    printf("\n\n");
    printf("<inv_DCT data>\n");
    for (int i = 248; i < 256; i++)
    {
        for (int j = 184; j < 192; j++)
        {
            printf("%d\t", DCT_matrix_inv[i][j]);
        }
        printf("\n");
    }


    //MSE
    double mse = 0;
    for (int p = 0; p < 64; p++)
    {
        for (int k = 0; k < 64; k++)
        {
            for (int i = 0; i < 8; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    mse += (((double)1 / (HEIGHT * WIDTH)) * pow(((double)input_data[(p * 8) + i][(k * 8) + j] - (double)DCT_matrix_inv[(p * 8) + i][(k * 8) + j]), 2));
                }
            }
        }
    }

    printf("\n\n\n");
    printf("MSE : ");
    printf("%lf", mse); // 11984 -> psnr:13





    //file write
    FILE* WriteFile;
    WriteFile = fopen("comp2_lena.raw", "wb");
    fwrite(DCT_ch, sizeof(unsigned char), WIDTH * HEIGHT, WriteFile);

    if (WriteFile != NULL)
        fclose(WriteFile);



}







void zig_move_right(int* zig_i, int* zig_j, int* flag, int* temp)
{
    ++(*zig_j);

    *flag = 2;

    if ((*temp) == 3 && (*flag) == 3) *flag = 4;

    if (*temp >= 4) *flag = 4;



}
void zig_move_down(int* zig_i, int* zig_j, int* flag, int* temp)
{
    ++(*zig_i);

    *flag = 4;
    if (*temp >= 4) *flag = 2;
}
void zig_move_diag_down(int* zig_i, int* zig_j, int* flag, int* temp)
{
    ++(*zig_i);
    --(*zig_j);

    if (*temp <= 3)
    {
        if (*zig_j == 0) *flag = 3;
    }
    else if (*temp >= 4)
    {
        if (*zig_i == 7) *flag = 3;
    }
}
void zig_move_diag_up(int* zig_i, int* zig_j, int* flag, int* temp)
{
    --(*zig_i);
    ++(*zig_j);

    if (*temp <= 2)
    {
        if (*zig_i == 0)
        {
            *flag = 1;
            (*temp)++;
        }
    }

    else if (*temp >= 3)
    {
        if (*zig_j == 7)
        {
            *flag = 1;
            (*temp)++;
        }
    }

}




