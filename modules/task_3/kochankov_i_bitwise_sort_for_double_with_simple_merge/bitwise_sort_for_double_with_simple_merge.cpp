// Copyright 2020 Kochankov Ilya
#include "../../../modules/task_3/kochankov_i_bitwise_sort_for_double_with_simple_merge/bitwise_sort_for_double_with_simple_merge.h"
#include <mpi.h>
#include <math.h>
#include <iostream>
#include <string>
#include <random>
#include <ctime>
#include <algorithm>
#include <vector>

using namespace std;

std::vector<double> getRandomVector(int size) {
   std::vector<double> vec(size);
   std::mt19937 gen;
   gen.seed(static_cast<unsigned int>(time(0)));
   for (int i = 0; i < size; i++)
      vec[i] = gen() / 10000;
   return vec;
}

std::vector<double> linear_bitwise_sort(const std::vector<double>& vect) {
   double max_number = vect[std::distance(vect.begin(), max_element(vect.begin(), vect.end()))];
   int digits_above_zero = get_digit_number_above_zero(max_number);
   int max_digits_below_zero = 0;
   int digits_below_zero;

   for (auto element: vect) {
      digits_below_zero = get_digit_number_below_zero(element);
      if (digits_below_zero > max_digits_below_zero) {
         max_digits_below_zero = digits_below_zero;
      }
   }

   vector<double> result(vect.size());
   copy(vect.begin(), vect.end(), result.begin());

   for (int i = -(max_digits_below_zero); i <= digits_above_zero; i++) {
      result = bitwise_sort(result, i);
   }

   return result;
}

std::vector<double> bitwise_sort(const std::vector<double>& vect, int discharge) {
   vector<vector<double>> baskets(10);

   fill(baskets.begin(), baskets.end(), vector<double>(0));

   for (auto element: vect) {
      baskets[get_digit(element, discharge)].push_back(element);
   }

   vector<double> result;
   for (auto basket: baskets){
      for (auto element: basket){
         result.push_back(element);
      }
   }

   return result;
}

int get_digit(double number, int discharge) {
   if (discharge < 0){
      return long(number * pow(10, -discharge)) % 10;
   }
   double mask = pow(10, discharge);
   double tmp = number / mask;
   return int(tmp) % 10;
}

int get_digit_number_above_zero(int number) {
   int digit = 0;
   while (number > 0) {
         number = number / 10;
         digit++;
   }
   return digit;
}

int get_digit_number_below_zero(double number) {
   auto s = to_string(number);
   if (s.find('.')) {
      return -(s.find('.') - s.size()) - 1;
   }else
      return 0;
}