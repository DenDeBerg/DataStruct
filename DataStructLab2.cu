#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cuda_runtime.h>
#include <mkl.h>
#include <device_launch_parameters.h>

// Создание матриц со случайным заполнением числами целого типа
void generateMatrix(float* matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            matrix[i * size + j] = static_cast<float>(rand()) / RAND_MAX;
        }
    }
}

// Вспомогательная функция по перемножениию матриц с использованием CUDA
__global__ void cudaMatrixMultiply(float* A, float* B, float* C, int size) {
    __shared__ float As[16][16];
    __shared__ float Bs[16][16];

    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int j = blockIdx.y * blockDim.y + threadIdx.y;
    int k = 0;

    float sum = 0.0f;

    for (int ti = threadIdx.x; ti < size; ti += blockDim.x) {
        As[threadIdx.y][threadIdx.x] = A[ti * size + j];
        Bs[threadIdx.y][threadIdx.x] = B[i * size + ti];
        __syncthreads();

        for (int k = 0; k < 16; k++) {
            sum += As[threadIdx.y][k] * Bs[k][threadIdx.x];
        }

        __syncthreads();
    }

    if (i < size && j < size) {
        C[i * size + j] = sum;
    }
}

// Функция по перемножениию матриц с использованием CUDA
void matrixMultiplyCUDA(float* A, float* B, float* C, int size) {
    float* d_A;
    float* d_B;
    float* d_C;

    cudaMalloc((void**)&d_A, size * size * sizeof(float));
    cudaMalloc((void**)&d_B, size * size * sizeof(float));
    cudaMalloc((void**)&d_C, size * size * sizeof(float));

    cudaMemcpy(d_A, A, size * size * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, B, size * size * sizeof(float), cudaMemcpyHostToDevice);

    dim3 threadsPerBlock(16, 16);
    dim3 numBlocks(size / threadsPerBlock.x, size / threadsPerBlock.y);

    cudaMatrixMultiply << <numBlocks, threadsPerBlock >> > (d_A, d_B, d_C, size);

    cudaMemcpy(C, d_C, size * size * sizeof(float), cudaMemcpyDeviceToHost);

    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);
}

// Функции перемножения матриц с использованием формул из линейной алгебры
void matrixMultiplyNaive(float* A, float* B, float* C, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            for (int k = 0; k < size; k++) {
                C[i * size + j] += A[i * size + k] * B[k * size + j];
            }
        }
    }
}

// Функция перемножения матриц с использованием BLAS
void matrixMultiplyBLAS(float* A, float* B, float* C, int size) {
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, size, size, size, 1.0f, A, size, B, size, 0.0f, C, size);
}


int main() {
    int size = 1048;
    float* A = new float[size * size];
    float* B = new float[size * size];
    float* C_naive = new float[size * size];
    float* C_blas = new float[size * size];
    float* C_cuda = new float[size * size];

    generateMatrix(A, size);
    generateMatrix(B, size);
    
    matrixMultiplyNaive(A, B, C_naive, size);
    matrixMultiplyBLAS(A, B, C_blas, size);
    matrixMultiplyCUDA(A, B, C_cuda, size);

    clock_t start, end;

    // Метод с использованием формул из линейной алгебры
    start = clock();
    matrixMultiplyNaive(A, B, C_naive, size);
    end = clock();
    double naive_time = static_cast<double>(end - start) / CLOCKS_PER_SEC;
    double naive_mflops = 2.0 * size * size * size / (1024 * 1024 * naive_time);

    // Метод с использованием MKL
    start = clock();
    matrixMultiplyBLAS(A, B, C_blas, size);
    end = clock();
    double blas_time = static_cast<double>(end - start) / CLOCKS_PER_SEC;
    double blas_mflops = 2.0 * size * size * size / (1024 * 1024 * blas_time);

    // Метод с использованием CUDA
    start = clock();
    matrixMultiplyCUDA(A, B, C_cuda, size);
    cudaDeviceSynchronize();
    end = clock();
    double cuda_time = static_cast<double>(end - start) / CLOCKS_PER_SEC;
    double cuda_mflops = 2.0 * size * size * size / (1024 * 1024 * cuda_time);

    std::cout << "Performance and time:" << std::endl;
    std::cout << "Naive method: " << naive_mflops << " MFlops, time: " << naive_time << " seconds" << std::endl;
    std::cout << "MKL method: " << blas_mflops << " MFlops, time: " << blas_time << " seconds" << std::endl;
    std::cout << "CUDA method: " << cuda_mflops << " MFlops, time: " << cuda_time << " seconds" << std::endl;

    bool equal = true;
    for (int i = 0; i < size * size; i++) {
        if (C_naive[i] != C_blas[i] || C_naive[i] != C_cuda[i]) {
            equal = false;
            break;
        }
    }
    if (equal) {
        std::cout << "Results are equal" << std::endl;
    }
    else {
        std::cout << "Results are not equal" << std::endl;
    }

    delete[] A;
    delete[] B;
    delete[] C_naive;
    delete[] C_blas;
    delete[] C_cuda;


    return 0;
}