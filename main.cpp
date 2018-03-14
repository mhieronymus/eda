#include <boost/cstdint.hpp>
#include <iostream>

#include "Minimizer/SampleSpace.h"
#include "likelihood/TestFunctions.h"
#include "Minimizer/MinimizerResult.h"


int main(int argc, char* argv[]) {
    
    uint32_t n_iterations = 1000000;
    uint32_t max_points = n_iterations+2;
    uint32_t seed = 1025;
    uint32_t ndims = 2;
    // Try the eggholder function
    TestFunctions test_func("eggholder", ndims);
    SampleSpace sampler(n_iterations, max_points, seed);
    v_d lower_bounds(ndims);
    v_d upper_bounds(ndims);
    lower_bounds[0] = -512;
    lower_bounds[1] = -512;
    upper_bounds[0] = 512;
    upper_bounds[1] = 512;
    MinimizerResult result = sampler.Minimize(test_func, lower_bounds, 
        upper_bounds);
    std::cout << std::endl << "Best fit for " << result.function_name 
        << " with " << result.minimizer_name << " is " 
        << result.best_fit << std::endl;
    
    // Townsend function
    test_func.set_func("townsend", ndims);
    lower_bounds[0] = -2.25;
    lower_bounds[1] = -2.5;
    upper_bounds[0] = 2.5;
    upper_bounds[1] = 1.75;
    result = sampler.Minimize(test_func, lower_bounds, 
        upper_bounds);
    std::cout << std::endl << "Best fit for " << result.function_name 
        << " with " << result.minimizer_name << " is " 
        << result.best_fit << std::endl;
        
    // Rosenbrock function
    test_func.set_func("rosenbrock", ndims);
    lower_bounds[0] = -6;
    lower_bounds[1] = -6;
    upper_bounds[0] = 6;
    upper_bounds[1] = 6;
    result = sampler.Minimize(test_func, lower_bounds, 
        upper_bounds);
    std::cout << std::endl << "Best fit for " << result.function_name 
        << " with " << result.minimizer_name << " is " 
        << result.best_fit << std::endl;
        
    // Himmelblau's function
    test_func.set_func("himmelblau", ndims);
    result = sampler.Minimize(test_func, lower_bounds, 
        upper_bounds);
    std::cout << std::endl << "Best fit for " << result.function_name 
        << " with " << result.minimizer_name << " is " 
        << result.best_fit << std::endl;
        
    // Gaussian shell 
    test_func.set_func("gaussian_shell", ndims);
    result = sampler.Minimize(test_func, lower_bounds, 
        upper_bounds);
    std::cout << std::endl << "Best fit for " << result.function_name 
        << " with " << result.minimizer_name << " is " 
        << result.best_fit << std::endl;
    
    return 0;
}