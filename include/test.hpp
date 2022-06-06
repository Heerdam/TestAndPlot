#ifndef TEST_HPP
#define TEST_HPP

#include <functional>
#include <memory>
#include <vector>
#include <tuple>
#include <string>
#include <chrono>
#include <iostream>
#include <optional>

#include <sciplot/sciplot.hpp>

#include "../include/tsc_x86.h"

#include "../include/pr_image.hpp"
#include "../include/ops_counter.hpp"
#include "../include/pathresolver.hpp"

enum class PlotType : size_t{
    PERFORMANCE_PLOT, RUNTIME_PLOT
};

enum class PlotScaling : size_t {
    IMAGE, CORNERS
};

enum class ImageType : size_t {
    CHECKERBOARD, RANDOM
};

template<class T, class S>
struct void_t {};

template<template<typename, typename> class>
struct has_memory_layout_s : public std::true_type {};

template<>
struct has_memory_layout_s<void_t> : public std::false_type {};

template<template<typename, typename> class T>
constexpr bool has_memory_layout = has_memory_layout_s<T>::value;

template<PlotType TYPE = PlotType::PERFORMANCE_PLOT, PlotScaling SCALING = PlotScaling::IMAGE, ImageType IMAGE = ImageType::RANDOM>
struct Test_Parameters {

    Test_Parameters() = default;

    //names
    std::string plot_name = "";
    std::string category_name = "";

    //runs
    size_t warmup_runs = 3;
    size_t measure_runs = 15;
     
    //size (both)
    size_t img_min_size = 500;
    size_t img_max_size = 5000;
    size_t img_step = 500;

    //corners (checkerboard)
    size_t checkerboard_box_count_min = 10;
    size_t checkerboard_box_count_max = 90;
    size_t checkerboard_box_size_step = 10;
    size_t checkerbord_image_border = 5;

    int threshold = 90;
    size_t seed = 0;

    template<PlotType T, PlotScaling S, ImageType I>
    Test_Parameters(const Test_Parameters<T, S, I>& o) {
        plot_name = o.plot_name;
        category_name = o.category_name;
        warmup_runs = o.warmup_runs;
        measure_runs = o.measure_runs;
        img_min_size = o.img_min_size;
        img_max_size = o.img_max_size;
        img_step = o.img_step;
        checkerboard_box_count_min = o.checkerboard_box_count_min;
        checkerboard_box_count_max = o.checkerboard_box_count_max;
        checkerboard_box_size_step = o.checkerboard_box_size_step;
        checkerbord_image_border = o.checkerbord_image_border;
        threshold = o.threshold;
        seed = o.seed;
    } 

};//Test_Parameters

//sudo chrt 99 ./<program>
class Tester {

    using Now = std::chrono::time_point<std::chrono::steady_clock>;

    using Params_t = std::unordered_map<std::string, std::vector<std::tuple<std::vector<double>, std::vector<size_t>, std::string, PlotType, PlotScaling, ImageType>>>;
    Params_t perf_times;

    size_t cc = 0;

    template<template<typename, typename, bool> class FUNC, PlotType TYPE, PlotScaling SCALING, ImageType IMAGE, template<PlotType, PlotScaling, ImageType> class PARAM>
    size_t measure_cycles_O0(const PARAM<TYPE, SCALING, IMAGE>& par, const cv::Mat& im) {
        ssize_t size;
        for (int i = 0; i < par.warmup_runs; ++i) {
            if constexpr (std::is_pointer_v<decltype(FUNC<ssize_t, unsigned char, false>::eval(im.data, im.cols, im.rows, im.step, par.threshold, &size))>) {
                auto* kps = FUNC<ssize_t, unsigned char, false>::eval(im.data, im.cols, im.rows, im.step, par.threshold, &size);
                free(kps);
            } else {
                auto kps = FUNC<ssize_t, unsigned char, false>::eval(im.data, im.cols, im.rows, im.step, par.threshold, &size);
            }
        }

        size_t cyclec = 0;
        for (int32_t i = 0; i < par.measure_runs; ++i) {
            size_t start = start_tsc();
            if constexpr (std::is_pointer_v<decltype(FUNC<ssize_t, unsigned char, false>::eval(im.data, im.cols, im.rows, im.step, par.threshold, &size))>) {
                auto* kps = FUNC<ssize_t, unsigned char, false>::eval(im.data, im.cols, im.rows, im.step, par.threshold, &size);
                free(kps);
            } else {
                auto kps = FUNC<ssize_t, unsigned char, false>::eval(im.data, im.cols, im.rows, im.step, par.threshold, &size);
            }
            cyclec += stop_tsc(start);
        }

        return cyclec;
    }

    template<template<typename, typename, bool> class FUNC, class MEMORY, PlotType TYPE, PlotScaling SCALING, ImageType IMAGE, template<PlotType, PlotScaling, ImageType> class PARAM>
    size_t measure_cycles_O0(const PARAM<TYPE, SCALING, IMAGE>& par, const cv::Mat& im, MEMORY mem) {
        ssize_t size;
        for (int i = 0; i < par.warmup_runs; ++i) {
            if constexpr (std::is_pointer_v<decltype(FUNC<ssize_t, unsigned char, false>::eval(im.data, im.cols, im.rows, im.step, par.threshold, &size))>) {
                auto* kps = FUNC<ssize_t, unsigned char, false>::eval(im.data, mem, im.cols, im.rows, im.step, par.threshold, &size);
                free(kps);
            } else {
                auto kps = FUNC<ssize_t, unsigned char, false>::eval(im.data, mem, im.cols, im.rows, im.step, par.threshold, &size);
            }
        }

        size_t cyclec = 0;
        for (int32_t i = 0; i < par.measure_runs; ++i) {
            size_t start = start_tsc();
            if constexpr (std::is_pointer_v<decltype(FUNC<ssize_t, unsigned char, false>::eval(im.data, im.cols, im.rows, im.step, par.threshold, &size))>) {
                auto* kps = FUNC<ssize_t, unsigned char, false>::eval(im.data, mem, im.cols, im.rows, im.step, par.threshold, &size);
                free(kps);
            } else {
                auto kps = FUNC<ssize_t, unsigned char, false>::eval(im.data, mem, im.cols, im.rows, im.step, par.threshold, &size);
            }
            cyclec += stop_tsc(start);
        }

        return cyclec;
    }

public:

    Tester() = default;
    Tester(const Tester&) = delete;

    template<template<typename, typename, bool> class FUNC, template<typename, typename> class MEMLAYOUT, PlotType TYPE, PlotScaling SCALING, ImageType IMAGE, template<PlotType, PlotScaling, ImageType> class PARAM>
    void run_test(const PARAM<TYPE, SCALING, IMAGE>& par) {

        static_assert(
                SCALING == PlotScaling::CORNERS && IMAGE == ImageType::CHECKERBOARD || 
                SCALING == PlotScaling::IMAGE && IMAGE == ImageType::CHECKERBOARD ||
                SCALING == PlotScaling::IMAGE && IMAGE == ImageType::RANDOM, "corner scaling only works with checkerboard");

        if constexpr(TYPE == PlotType::RUNTIME_PLOT){

            Files files = Files("tester");
            std::vector<double> mt;
            std::vector<size_t> ms;

            const std::string cat = !par.category_name.empty() ? "Runtime plot: " + par.category_name : "Runtime plot: cat_empty";

            const size_t min = SCALING == PlotScaling::CORNERS ? par.checkerboard_box_count_min : par.img_min_size;
            const size_t max = SCALING == PlotScaling::CORNERS ? par.checkerboard_box_count_max : par.img_max_size;
            const size_t step = SCALING == PlotScaling::CORNERS ? par.checkerboard_box_size_step : par.img_step;

            const double frac = 1. / (double(max - min)/double(step));
            double progress = 0.;

            ssize_t size = 0;

            for (size_t k = min; k <= max; k+=step) {

                std::cout << "\33[2K\r" << "Test: [Runtime][" << par.plot_name << "] |";

                for(double p = 0.; p <= progress; p+=0.1)
                    std::cout << "x";

                std::cout << "| " << int(100. * progress) << "%" << " [" << size << "]   " << std::flush;

                PRImage<ssize_t, unsigned char> image;
                if constexpr(IMAGE == ImageType::CHECKERBOARD && SCALING == PlotScaling::IMAGE)
                    image.asCheckerboard(par.img_min_size, par.img_min_size, k, k, par.checkerbord_image_border);
                else if constexpr(IMAGE == ImageType::CHECKERBOARD && SCALING == PlotScaling::CORNERS)
                    image.asCheckerboard(par.img_min_size, par.img_min_size, k, k, par.checkerbord_image_border);
                else image.asRandomImage(files.root_data() /= "samples.png", k, k, par.seed);

                const cv::Mat im = image.toMat();
                cv::imwrite(files.data() /= "cb12.png", im);

                //std::cout << "B: " << par.img_min_size / k << std::endl;

                for (int i = 0; i < par.warmup_runs; ++i) {
                    if constexpr (std::is_pointer_v<decltype(FUNC<ssize_t, unsigned char, false>::eval(im.data, im.cols, im.rows, im.step, par.threshold, &size))>) {
                        auto* kps = FUNC<ssize_t, unsigned char, false>::eval(im.data, im.cols, im.rows, im.step, par.threshold, &size);
                        free(kps);
                    } else {
                        auto kps = FUNC<ssize_t, unsigned char, false>::eval(im.data, im.cols, im.rows, im.step, par.threshold, &size);
                    }
                }
                Now start = std::chrono::steady_clock::now();
                for (int32_t i = 0; i < par.measure_runs; ++i) {
                    if constexpr (std::is_pointer_v<decltype(FUNC<ssize_t, unsigned char, false>::eval(im.data, im.cols, im.rows, im.step, par.threshold, &size))>) {
                        auto* kps = FUNC<ssize_t, unsigned char, false>::eval(im.data, im.cols, im.rows, im.step, par.threshold, &size);
                        free(kps);
                    } else {
                        auto kps = FUNC<ssize_t, unsigned char, false>::eval(im.data, im.cols, im.rows, im.step, par.threshold, &size);
                    }
                }
                const Now end = std::chrono::steady_clock::now();
                mt.push_back(std::chrono::duration<double>(end - start).count() / par.measure_runs);
                ms.push_back(k);

                progress += frac;

                //std::cout << std::endl;
                
            }

            std::string plotn = !par.plot_name.empty() ? par.plot_name : std::string("plot_") + std::to_string(cc++);
            perf_times[cat].push_back({ std::move(mt), std::move(ms), std::move(plotn), TYPE, SCALING, IMAGE });

            std::cout << "\33[2K\r" << "Test: [Runtime][" << par.plot_name << "] done!" << std::endl;

        } else if constexpr(TYPE == PlotType::PERFORMANCE_PLOT){

            Files files = Files("tester");

            std::vector<double> ps;
            std::vector<size_t> ms;

            const std::string cat = !par.category_name.empty() ? "Performance plot: " + par.category_name : "Performance plot: cat_empty";

            const size_t min = SCALING == PlotScaling::CORNERS ? par.checkerboard_box_count_min : par.img_min_size;
            const size_t max = SCALING == PlotScaling::CORNERS ? par.checkerboard_box_count_max : par.img_max_size;
            const size_t step = SCALING == PlotScaling::CORNERS ? par.checkerboard_box_size_step : par.img_step;

            const double frac = 1. / (double(max - min)/double(step));
            double progress = 0.;

            for (size_t k = min; k <= max; k+=step) {

                std::cout << "\33[2K\r" << "Test: [Performance][" << par.plot_name << "] |";

                for(double p = 0.; p <= progress; p+=0.1)
                    std::cout << "x";

                std::cout << "| " << int(100. * progress) << "%" << "  " << std::flush;

                PRImage<ssize_t, unsigned char> image;
                if constexpr(IMAGE == ImageType::CHECKERBOARD && SCALING == PlotScaling::IMAGE)
                    image.asCheckerboard(par.img_min_size, par.img_min_size, k, k, par.checkerbord_image_border);
                else if constexpr(IMAGE == ImageType::CHECKERBOARD && SCALING == PlotScaling::CORNERS)
                    image.asCheckerboard(par.img_min_size, par.img_min_size, k, k, par.checkerbord_image_border);
                else image.asRandomImage(files.root_data() /= "samples.png", k, k, par.seed);

                const cv::Mat im = image.toMat();

                size_t cyclec = 0;
                Counter count;

                if constexpr (has_memory_layout<MEMLAYOUT>){

                    auto mem1 = MEMLAYOUT<ssize_t, unsigned char>::eval(im);

                    //------------- measure cycles -------------
                    cyclec = measure_cycles_O0<FUNC, MEMLAYOUT, TYPE, SCALING, IMAGE, PARAM>(par, mem1, im);
                    
                    //------------- measure ops -------------
                    Counter count;
                    count.reset();

                    using INT = INTCounter<ssize_t, Counter::INT>;
                    using CHAR = INTCounter<unsigned char, Counter::BYTE>;

                    INT ssize;

                    auto mem2 = MEMLAYOUT<INT, CHAR>::eval(im);

                    std::vector<INTCounter<unsigned char, Counter::BYTE>> ima;
                    for(size_t i = 0; i < image.width() * image.height(); ++i)
                        ima.emplace_back( image.data()[i] );

                    count.reset();
                    
                    if constexpr (std::is_pointer_v<decltype(FUNC<INT, CHAR, true>::eval(ima.data(), mem2, { image.width() }, { image.height() }, { image.step() }, {ssize_t(par.threshold) }, &ssize))>) {
                        auto* kps = FUNC<INT, CHAR, true>::eval(ima.data(), mem2, { image.width() }, { image.height() }, { image.step() }, {ssize_t(par.threshold) }, &ssize);
                        free(kps);
                    } else {
                        auto kps = FUNC<INT, CHAR, true>::eval(ima.data(), mem2, { image.width() }, { image.height() }, { image.step() }, {ssize_t(par.threshold) }, &ssize);
                    }

                } else {
                    //------------- measure cycles -------------
                    cyclec = measure_cycles_O0<FUNC, TYPE, SCALING, IMAGE, PARAM>(par, im);
                    
                    //------------- measure ops -------------                   
                    count.reset();

                    using INT = INTCounter<ssize_t, Counter::INT>;
                    using CHAR = INTCounter<unsigned char, Counter::BYTE>;

                    INT ssize;

                    std::vector<INTCounter<unsigned char, Counter::BYTE>> ima;
                    for(size_t i = 0; i < image.width() * image.height(); ++i)
                        ima.emplace_back( image.data()[i] );

                    count.reset();
                    
                    if constexpr (std::is_pointer_v<decltype(FUNC<INT, CHAR, true>::eval(ima.data(), { image.width() }, { image.height() }, { image.step() }, { static_cast<unsigned char>(par.threshold) }, &ssize))>) {
                        auto* kps = FUNC<INT, CHAR, true>::eval(ima.data(), { image.width() }, { image.height() }, { image.step() }, { static_cast<unsigned char>(par.threshold) }, &ssize);
                        free(kps);
                    } else {
                        auto kps = FUNC<INT, CHAR, true>::eval(ima.data(), { image.width() }, { image.height() }, { image.step() }, { static_cast<unsigned char>(par.threshold) }, &ssize);
                    }
                }

                size_t ops = 0;
                //ops += count.getCount(Counter::INT, Counter::ASSIGN);
                ops += count.getCount(Counter::INT, Counter::ADD);
                ops += count.getCount(Counter::INT, Counter::SUB);
                ops += count.getCount(Counter::INT, Counter::MUL);
                ops += count.getCount(Counter::INT, Counter::DIV);
                ops += count.getCount(Counter::INT, Counter::COMP);
                ops += count.getCount(Counter::INT, Counter::BYTEOPS);

                //ops += count.getCount(Counter::BYTE, Counter::ASSIGN);
                ops += count.getCount(Counter::BYTE, Counter::ADD);
                ops += count.getCount(Counter::BYTE, Counter::SUB);
                ops += count.getCount(Counter::BYTE, Counter::MUL);
                ops += count.getCount(Counter::BYTE, Counter::DIV);
                ops += count.getCount(Counter::BYTE, Counter::COMP);
                ops += count.getCount(Counter::BYTE, Counter::BYTEOPS);

                ops += count.getCount(Counter::M256i, Counter::ADD);
                ops += count.getCount(Counter::M256i, Counter::SUB);
                ops += count.getCount(Counter::M256i, Counter::MUL);
                ops += count.getCount(Counter::M256i, Counter::DIV);
                ops += count.getCount(Counter::M256i, Counter::COMP);
                ops += count.getCount(Counter::M256i, Counter::BYTEOPS);

                
                const double cperf = double(cyclec) / double(par.measure_runs);
                const double perf = double(ops) / double(cperf);

                ps.push_back(perf);
                ms.push_back(k);

                progress += frac;

                count.reset();
                
            }

            std::string plotn = !par.plot_name.empty() ? par.plot_name : std::string("plot_") + std::to_string(cc++);
            perf_times[cat].push_back({ std::move(ps), std::move(ms), std::move(plotn), TYPE, SCALING, IMAGE });

            std::cout << "\33[2K\r" << "Test: [Performance][" << par.plot_name << "] done!" << std::endl;

        } else []<bool flag = false>() {static_assert(flag, "Plot type not implemented yet");}();
        
    }

    template<template<typename, typename, bool> class FUNC, PlotType TYPE, PlotScaling SCALING, ImageType IMAGE, template<PlotType, PlotScaling, ImageType> class PARAM>
    void run_test(const PARAM<TYPE, SCALING, IMAGE>& par) {
        run_test<FUNC, void_t, TYPE, SCALING, IMAGE, PARAM>(par);
    }

    void reset() {
        perf_times.clear();
    }

    /*finalize testing and print the plots*/
    void finalize(const std::string& _plot_name) {

        size_t px = 2;
        size_t py = perf_times.size() / 2 + perf_times.size()%2;
        //std::cout << py << std::endl;
        std::vector<std::vector<sciplot::PlotVariant>> plots;
        plots.emplace_back();

        size_t k = 0, l = 0;
        for(auto it = perf_times.cbegin(); it != perf_times.cend(); ++it){

            sciplot::Plot plot;
            plot.palette("jet");
            plot.fontSize(16);
            PlotType type = PlotType::PERFORMANCE_PLOT;
            PlotScaling scaling = PlotScaling::IMAGE;
            ImageType image = ImageType::CHECKERBOARD;

            const auto& vals = it->second;
            double maxRange = -std::numeric_limits<double>::infinity();

            for(size_t i = 0; i < vals.size(); ++i){
                const auto& [v, s, n, t, sc, it] = vals[i];

                //max value
                for(size_t i = 0; i < v.size(); ++i)
                    if(v[i] > maxRange)
                        maxRange = v[i];

                type = t;
                scaling = sc;
                image = it;
                plot.drawCurve(s, v).label(n).lineWidth(3.);
            }

            plot.xlabel(scaling == PlotScaling::IMAGE ? (image == ImageType::CHECKERBOARD ? "[Checkerboard] width [pixels]" : "[Random] width [pixels]") : "[boxes]").fontSize(16);
            plot.ylabel((type == PlotType::PERFORMANCE_PLOT ? "[cost/cycle]" : "[seconds]")).fontSize(16);
            plot.xtics().fontSize(14);
            plot.ytics().fontSize(14);
            plot.yrange(0., maxRange * 1.5);
            plot.grid().show();
            plot.legend().atTopRight();

            std::stringstream ss;
            ss << "set title \"" << it->first << "\" font \",18\"";
            plot.gnuplot(ss.str());

            plot.gnuplot("set bmargin 8");

            if(l == 1) plot.gnuplot("set lmargin 10");
            if(l == 0) plot.gnuplot("set rmargin 5");

            plot.gnuplot("set tmargin 5");
            plot.gnuplot("set border 4095");

            plots[k].push_back({ std::move(plot) });
            l++;
            if(l == 2){ 
                ++k;
                l = 0;
                plots.emplace_back();
            }
        }

        //inverse
        std::vector<std::vector<sciplot::PlotVariant>> plotsr;
        for(ssize_t i = plots.size() - 1; i >= 0 ; --i){
            std::vector<sciplot::PlotVariant> t;
            for(ssize_t j = plots[i].size() - 1; j >= 0 ; --j){
                t.push_back(std::move(plots[i][j]));
            }
            if(plots[i].empty()) continue;
            plotsr.push_back(std::move(t));
        }

        sciplot::Figure fig(plotsr);
        fig.size(px*800, py*620);
        fig.save(_plot_name);

        std::cout << "Plotting: done!" << std::endl;

    }

};

#endif
