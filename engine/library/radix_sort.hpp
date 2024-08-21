/// \file radix_sort.hpp
/// The radix_sort function is defined.
#pragma once

// dependencies

// libraries

//std
#include <cstdint>
#include <stdexcept>
#include <climits>
#include <random>
#include <cinttypes>

namespace ae {

    void lsb_radix_sort(const std::uint32_t *t_arrayRef, std::uint32_t *t_sorted, const std::size_t t_arraySize){

        // This radix sort is assuming the radix is 11 bits of the input value.
        std::uint32_t radix_bits = 11;
        std::uint32_t radix_hist = 1<<radix_bits;

        // Create the histograms for each of the 11 bit sections of the input number.
        std::uint32_t h0[radix_hist * 3];

        std::uint32_t *h1 = h0 + radix_hist;
        std::uint32_t *h2 = h1 + radix_hist;

        // Ensure the array is cleared.
        for (int i = 0; i < (radix_hist * 3); i++) {
            h0[i] = 0;
        }

        // Counting sort into radix histogram bins
        for(int i=0;i<t_arraySize;i++){
            h0[t_arrayRef[i] & 0x7FF]++;
            h1[(t_arrayRef[i] >> 11) & 0x7FF]++;
            h2[(t_arrayRef[i] >> 22)]++;
        }

        // Calculate the cumulative sum across the bins
        std::uint32_t h0_sum = 0, h1_sum = 0, h2_sum = 0;
        std::uint32_t temp_sum = 0;
        for (int i = 0; i < radix_hist; i++) {
            temp_sum = h0[i] + h0_sum;
            // I don't like -1 being used here, will lead to an overflow and most likely will cause issues.
            h0[i] = h0_sum - 1;
            h0_sum = temp_sum;

            temp_sum = h1[i] + h1_sum;
            h1[i] = h1_sum - 1;
            h1_sum = temp_sum;

            temp_sum = h2[i] + h2_sum;
            h2[i] = h2_sum - 1;
            h2_sum = temp_sum;
        }

        // Sort the array based the 11-LS bits.
        for (int i = 0; i < t_arraySize; i++) {

            std::uint32_t element = t_arrayRef[i];
            std::uint32_t pos = element & 0x7FF;

            t_sorted[++h0[pos]] = element;
        }

        std::uint32_t temp_sort[t_arraySize];
        // Sort the array based the next 11-LS bits.
        for (int i = 0; i < t_arraySize; i++) {
            std::uint32_t element = t_sorted[i];
            std::uint32_t pos = (element >> 11) & 0x7FF;

            temp_sort[++h1[pos]] = element;
        }

        // Sort the array based the 11-MS bits.
        for (int i = 0; i < t_arraySize; i++) {
            std::uint32_t element = temp_sort[i];
            std::uint32_t pos = element >> 22;

            t_sorted[++h2[pos]] = element;
        }

    }

    void parallel_lsb_radix_sort(const std::uint32_t *t_arrayRef, std::uint32_t *t_sorted, const std::size_t t_arraySize) {

        // const std::uint32_t t_numCores = 16;
        // const std::uint32_t radix_bits = 4;
        // const std::uint32_t radix_num_vals = 1 << radix_bits;
        // const std::uint32_t num_hists = 8; // Number of radix bits in max value of array members

        std::uint32_t t_numCores = 4;
        std::uint32_t radix_bits = 3;
        std::uint32_t radix_num_vals = 1 << radix_bits;
        std::uint32_t num_hists = 3; // Number of radix bits in max value of array members
        std::uint32_t hist_mask = (radix_num_vals-1);

        // ensure array can be split into sections
        if (t_arraySize % t_numCores) {
            throw std::runtime_error(
                    "parallel_lsb_radix_sort: Array size is not dividable my number of processors in parallel!");
        }
        const std::uint32_t range_size = t_arraySize / t_numCores;

        std::uint32_t hist[t_numCores][num_hists][radix_num_vals];
        std::uint32_t offset[t_numCores][num_hists][radix_num_vals];
        for(std::uint32_t i=0;i<t_numCores;i++)
        {
            for(std::uint32_t j=0;j<num_hists;j++)
            {
                for(std::uint32_t k=0;k<radix_num_vals;k++)
                {
                    hist[i][j][k] = 0;
                    offset[i][j][k] = 0;
                }
            }
        }

        // Create an array to hold the values from the previous round of sorting.
        std::uint32_t intermediate_sorted[t_arraySize];
        for(std::uint32_t i=0;i<t_arraySize;i++)
        {
            intermediate_sorted[i] = t_arrayRef[i];
        }

        // Repeat the algorithm for each of the histograms starting from LSB to MSB.
        for(std::uint32_t j = 0; j < num_hists; j++){

            std::uint32_t hist_shift = radix_bits * j;

            // Emulate number of cores working on different portions of the dataset
            for (std::uint32_t i = 0; i < t_numCores; i++) {
                std::size_t start_idx = range_size * i;
                std::size_t end_idx = range_size * (i + 1);

                // Create histograms for different "LSB" Histograms, this is also done by different cores in parallel
                for (std::size_t k = start_idx; k < end_idx; k++) {
                    // Add one to the portion of the histogram corresponding to the array value masked by the radix.
                    hist[i][j][(t_arrayRef[k] >> hist_shift) & hist_mask]++;
                }
            }

            // Calculate the offset for each of the histogram values.
            for (std::uint32_t i = 0; i < t_numCores; i++) {
                for (std::size_t k = 0; k < radix_num_vals; k++) {
                    offset[i][j][k] = 0;

                    // Loop through values of all cores and sum all the counts for values less than the current value.
                    for(std::uint32_t lt_k = 0; lt_k<k; lt_k++){
                        for(int all_cores = 0; all_cores<t_numCores; all_cores++){
                            offset[i][j][k] += hist[all_cores][j][lt_k];
                        }
                    }

                    // Loop through all values from cores that took ranges less than the range of the current core
                    if(i>0)
                    {
                        for(std::uint32_t lt_i = 0; lt_i<i; lt_i++){
                            offset[i][j][k] += hist[lt_i][j][k];
                        }
                    }
                }
            }
        }

        // Sort based upon the created histograms
        for(std::uint32_t j = 0; j < num_hists; j++)
        {
            std::uint32_t hist_shift = (radix_bits * j);

            // Emulate number of cores working on different portions of the dataset
            for (std::uint32_t i = 0; i < t_numCores; i++) {
                std::size_t start_idx = range_size * i;
                std::size_t end_idx = range_size * (i + 1);

                // Create histograms for different "LSB" Histograms, this is also done by different cores in parallel
                for (std::uint32_t k = start_idx; k < end_idx; k++) {
                    // The masked value gets the index into the offset array that represents where the value needs to be
                    // placed in the final sort.
                    std::size_t offset_index = (intermediate_sorted[k] >> hist_shift) & hist_mask;

                    // Put the value into its final place in the sort for the current portion of the sort.
                    t_sorted[offset[i][j][offset_index]] = intermediate_sorted[k];

                    // Increment the offset for the current value so if there are multiples of a specific value it gets
                    // placed after the current one in the range.
                    offset[i][j][offset_index]++;
                }
            }

            if(j != num_hists-1)
            {
                for(int i=0;i<t_arraySize;i++)
                {
                    intermediate_sorted[i] = t_sorted[i];
                }
            }

        }
        printf("Done Parallel Sorting.\n");
    }

    void test_radix_sort(){
        std::size_t const num_random_values = 16;
        std::random_device random_device;
        std::mt19937 random_engine(random_device());
        //std::uniform_int_distribution<uint32_t> uint32_distribution(0, UINT32_MAX);
        std::uniform_int_distribution<uint32_t> uint32_distribution(0, 255);

        std::uint32_t numbers[num_random_values] = {0};

        printf("Random Array Values:\n");
        for (size_t i = 0; i < num_random_values; ++i)
        {
            numbers[i] = uint32_distribution(random_engine);
            printf("%" PRIu32 "\n", numbers[i]);
        }

        // std::size_t const num_random_values = 8;
        // std::uint32_t numbers[num_random_values] = {253,75,105,178,45,128,88,200};

        for (size_t i = 0; i < num_random_values; ++i)
        {
            printf("%" PRIu32 "\n", numbers[i]);
        }
        printf("\n\n");

        std::uint32_t sorted[num_random_values];
        lsb_radix_sort(numbers,sorted,num_random_values);

        printf("Serial Sorted Array:\n");
        for(size_t i = 0; i < num_random_values; ++i){
            printf("%" PRIu32 "\n", sorted[i]);
        }
        printf("\n\n");


        std::uint32_t parallel_sorted[num_random_values];
        parallel_lsb_radix_sort(numbers,parallel_sorted,num_random_values);

        printf("Parallel Sorted Array:\n");
        for(size_t i = 0; i < num_random_values; ++i){
            printf("%" PRIu32 "\n", parallel_sorted[i]);
        }
        printf("\n\n");
    }

} // namespace ae