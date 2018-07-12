/**
 * @brief
 * Interface of 
 * Accelerated Parameter Estimation with DALEχ
 * by Scott, Daniel F.
 * https://arxiv.org/abs/1705.02007
 * see https://github.com/danielsf/Dalex
 * 
 * Author: Maicon Hieronymus <mhierony@students.uni-mainz.de>
 */
#include "Minimizer/DalexMinimizer.h"
#include <boost/foreach.hpp>
#include <boost/random.hpp>
#include <boost/nondet_random.hpp>
#include <fstream>

/** Constructor and destructor **/
DalexMinimizer::DalexMinimizer(
    int max_iter,
    int max_points,
    int seed,
    bool dump_points) : Minimizer(0, max_iter, 0, max_points, seed, dump_points)
{
    seed_ = seed;
}

/** Return the name of this class.
 *
 *  \return     Name of this class.
 */
std::string DalexMinimizer::get_name() {
    return ("DalexMinimizer");
}

/** Function to map from the unit hypercube to Theta in the physical space.
 *
 * \param cube              Hypercube coordinates of point that
 *                          shall be evaluated.
 * \param nDims             Dimensionallity of parameter space in terms of
 *                          free parameter for minimization
 * */
v_d DalexMinimizer::to_physics(
    v_d cube,
    uint32_t nDims) {

    v_d theta;
    
    for (int i=0; i<nDims; i++) {
        theta.push_back(this->lower_bnds[i]
        + (this->upper_bnds[i] - this->lower_bnds[i])
        * cube[i]);
    }

    return theta;
}

/** Configure the dalex object and execute it.
 * 
 * \param nDims             Dimensionallity of parameter space in terms of
 *                          free parameter for minimization
 * */
void DalexMinimizer::execute(
    uint32_t nDims){

    // Class needed by Dalex
    class my_chisq_fn : public chisquared{

    public:

        // the constructor should call the constructor for
        // chisquared, passing in one int for the dimensionality
        // of the likelihood function
        my_chisq_fn(int nDims) : chisquared(nDims){
            _called = 0;
            _dim = nDims;
            _best_params.clear();
            for(uint32_t i = 0; i < nDims; i++)
                _best_params.push_back(0);
            
            _best = std::numeric_limits<double>::max();
        }

        void set_fun(TestFunctions *llh_func) {
                _llh_func = llh_func;
        }

        // you must define an operator that accepts an array_1d<double>
        // representing the point in parameter space and returns
        // a double representing the chi^2 value at that point
        virtual double operator()(const array_1d<double> &in){
            v_d theta;
            for (int i=0; i<_dim; i++) {
                theta.push_back(in.get_data(i));
            }
            _called++;
            double ans = _llh_func->get_neg_lh(theta);
            if(ans < _best) {
                _best = ans;
                _best_params = theta;
            }
            // std::cout << ans << "\n";
            return ans;
        }

        v_d get_best_params() {return _best_params;}
        double get_chimin() {return _best;}
    private:
        TestFunctions *_llh_func;

        double _best;
        v_d _best_params;
    };
    
    my_chisq_fn chifn(nDims);
    array_1d<double> min, max;
    for(uint32_t i = 0; i < nDims; i++) {
        min.set(i, lower_bnds[i]);
        max.set(i, upper_bnds[i]);
    }
    chifn.set_fun(test_func_);

    dalex_driver dalex_d;
    dalex_d.set_deltachi(12.03); // What value is reasonable?
    dalex_d.set_seed(seed_);
    dalex_d.set_min(min);
    dalex_d.set_max(max);
    dalex_d.set_chisquared(&chifn);
    // std::string timing_name = "basic_timing.txt";
    // std::string output_name = "basic_output.txt";
    char timingname[200];
    strcpy( timingname, (base_dir_ + "basic_timing.txt").c_str() );
    dalex_d.set_timingname(timingname);
    char outname[200];
    strcpy( outname, (base_dir_ + "basic_output.txt").c_str() );

    dalex_d.set_outname(outname);
    dalex_d.initialize(12); 
    dalex_d.set_write_every(max_points_); 
    dalex_d.search(max_iter_); 

    result.best_fit = dalex_d.get_chimin();
    result.n_lh_calls = dalex_d.get_called();
    result.params_best_fit = chifn.get_best_params();
    std::cout << dalex_d.get_chimin() << ", " << chifn.get_chimin() << "\n";
    



    // dalex_initializer * initializer = new dalex_initializer();

    // chisq_wrapper * chifn_wrapped = new chisq_wrapper();
    // array_1d<double> min, max;
    // for(uint32_t i = 0; i < nDims; i++) {
    //     min.set(i, lower_bnds[i]);
    //     max.set(i, upper_bnds[i]);
    // }
    // chifn_wrapped->set_min(min);
    // chifn_wrapped->set_max(max);
    // chifn_wrapped->set_chisquared(); // What?
    // chifn_wrapped->set_deltachi(12.0);
    // chifn_wrapped->set_seed(seed_);
    // chifn_wrapped->initialize(nDims*2);
    // initializer->set_chifn(chifn_wrapped);
    // initializer->search();

    // std::cout << chifn_wrapped->get_called() << ", " 
    //     << chifn_wrapped->chimin() << "\n";

    // delete chifn_wrapped;
    // delete initializer;

}

/** Required Minimize() function for every minimizer. Sets the bounds.
 *
 *  \param test_func        The function which shall be minimized
 *  \param lower_bounds     The lower bounds for each dimension
 *  \param upper_bounds     The upper bounds for each dimension
 *
 *  \return                 The result of the minimization
 * */
MinimizerResult
DalexMinimizer::Minimize(
    TestFunctions test_func,
    v_d lower_bounds,
    v_d upper_bounds ) {

    reset_calls();
    results.clear();
    upper_bnds = upper_bounds;
    lower_bnds = lower_bounds;
    test_func_ = &test_func;
    file_name_ = test_func_->get_name();

    // Execute DalexMinimizer
    execute(test_func_->get_ndims());

    result.minimizer_name = "DalexMinimizer";
    result.function_name = test_func_->get_name();
    return result;
}
