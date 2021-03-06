// Run TCG and throw a NaN in the preconditioner 

#include "linear_algebra.h"
#include "spaces.h"

int main() {
    // Setup the problem 
    auto setup = Unit::tcg <Real,Rm> ();
        
    // Problem setup
    setup.A = std::make_unique <Matrix>(
        Unit::Matrix <Real>::symmetric(setup.m,0));
    setup.b = std::make_unique <Vector> (
        Unit::Vector <Real>::basic(setup.m));

    // When we throw a NaN immediately, we should exit with a zero solution 
    {
        // Problem setup
        setup.B = std::make_unique <Unit::Operator <Real,Rm>::Nan> (
            Unit::Operator <Real,Rm>::Nan(
                Unit::Matrix <Real>::symmetric(setup.m,3),
                0));

        // Target solutions
        setup.x_star = std::make_unique<Vector>(
            Unit::Vector <Real>::zero(setup.m));
        setup.iter_star = 0;
        setup.stop_star = Optizelle::TruncatedStop::NanPreconditioner;

        // Tests
        setup.check_sol = true;
        setup.check_iter = true;
        setup.check_stop = true;
        setup.check_cp = true;

        // Check the solver 
        Unit::run_and_verify <Real,Rm> (setup);
    }

    // When we throw a NaN on the second call, we should have gotten through
    // the first iteration successfully.  This means that we should exit and
    // disregard any solution from the second iteration and just use the answer
    // from the first.
    { 
        // Run for a single iteration and grab the solution 
        setup.B = std::make_unique <Matrix>(
            Unit::Matrix <Real>::symmetric(setup.m,3));
        setup.iter_max = 1;

        // No tests
        Unit::reset_checks <Real,Rm> (setup);

        // Run the solver
        Unit::run_and_verify <Real,Rm> (setup);

        // Now, throw a NaN in the preconditioner after a single call
        setup.B = std::make_unique <Unit::Operator <Real,Rm>::Nan> (
            Unit::Operator <Real,Rm>::Nan(
                Unit::Matrix <Real>::symmetric(setup.m,3),
                1));
        setup.iter_max = 500;

        // Target solutions
        setup.x_star = std::make_unique<Vector>(*setup.x);
        setup.iter_star = 1;
        setup.stop_star = Optizelle::TruncatedStop::NanPreconditioner;

        // Tests
        Unit::reset_checks <Real,Rm> (setup);
        setup.check_sol = true;
        setup.check_iter = true;
        setup.check_stop = true;
        setup.check_cp = true;

        // Check the solver 
        Unit::run_and_verify <Real,Rm> (setup);
    }

    // Declare success
    return EXIT_SUCCESS;
}
