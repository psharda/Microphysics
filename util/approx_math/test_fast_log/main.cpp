#include <test_fast_log.H>

int main() {

    // Accuracy tests

    // Test values including edge cases and some typical values

    // test_fast_log_accuracy(-1.0_rt);
    // test_fast_log_accuracy(0.0_rt);
    test_fast_log_accuracy(0.001_rt);
    test_fast_log_accuracy(0.01_rt);
    test_fast_log_accuracy(0.1_rt);
    test_fast_log_accuracy(0.5_rt);
    test_fast_log_accuracy(1.0_rt);
    test_fast_log_accuracy(3.14_rt);
    test_fast_log_accuracy(5.0_rt);
    test_fast_log_accuracy(10.0_rt);
    test_fast_log_accuracy(15.0_rt);
    test_fast_log_accuracy(20.0_rt);
    test_fast_log_accuracy(30.0_rt);
    test_fast_log_accuracy(50.0_rt);
    test_fast_log_accuracy(100.0_rt);
    test_fast_log_accuracy(500.0_rt);
    // test_fast_log_accuracy(std::numeric_limits<amrex::Real>::infinity());


    std::cout << "Accuracy tests passed!" << std::endl;

    // Now performance test

    int iters = 5;
    amrex::Real test_value = 160.0_rt;
    test_fast_log_speed(100, iters, test_value);

    iters = 10;
    test_fast_log_speed(100, iters, test_value);

    iters = 20;
    test_fast_log_speed(100, iters, test_value);

    iters = 30;
    test_fast_log_speed(100, iters, test_value);

    iters = 50;
    test_fast_log_speed(100, iters, test_value);

    iters = 70;
    test_fast_log_speed(100, iters, test_value);
}
