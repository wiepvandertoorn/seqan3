// This example illustrates how we can reduce the usage of templates (or the amount of different instantiations) via
// type erasure. Having only one function generated for `algorithm()` is the only benefit of using `semialphabet_any`
// here. Of course this only makes sense for your application if the part of the program that is agnostic of the
// character representation (your equivalent of `algorithm()`) is substantially larger than the specific parts – and
// if compile-time and/or size of the exectuble are a concern.

#include <iostream>

#include <seqan3/alphabet/aminoacid/all.hpp>
#include <seqan3/alphabet/composite/semialphabet_any.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/range/views/convert.hpp>
#include <seqan3/range/views/to.hpp>

using seqan3::operator""_aa10murphy;
using seqan3::operator""_aa10li;

// Print is a template and gets instantiated two times because the behaviour is different for both types
template <typename rng_t>
void print(rng_t && r)
{
    seqan3::debug_stream << r << '\n';
}

// Algorithm is not a template, only one instance is generated by the compiler
// Type information is encoded via a run-time parameter
void algorithm(std::vector<seqan3::semialphabet_any<10> > & r, bool is_murphy)
{
    // Algorithm example that replaces rank 0 with rank 1
    for (auto & v : r)
        if (seqan3::to_rank(v) == 0)
            seqan3::assign_rank_to(1, v);

    // Here we reify the type for printing
    if (is_murphy)
        print(r | seqan3::views::convert<seqan3::aa10murphy>);
    else
        print(r | seqan3::views::convert<seqan3::aa10li>);
}

// Two instances of algo_pre exist
// They type erase the different arguments to the same type and encode the type information as a run-time parameter
void algo_pre(seqan3::aa10li_vector const & v)
{
    std::vector<seqan3::semialphabet_any<10> > tmp = v | seqan3::views::convert<seqan3::semialphabet_any<10>>
                                                       | seqan3::views::to<std::vector>;
    algorithm(tmp, false);
}

void algo_pre(seqan3::aa10murphy_vector const & v)
{
    std::vector<seqan3::semialphabet_any<10> > tmp = v | seqan3::views::convert<seqan3::semialphabet_any<10>>
                                                       | seqan3::views::to<std::vector>;
    algorithm(tmp, true);
}

int main()
{
    seqan3::aa10li_vector v1{"AVRSTXOUB"_aa10li};
    algo_pre(v1); // BIKBBBKCB

    seqan3::aa10murphy_vector v2{"AVRSTXOUB"_aa10murphy};
    algo_pre(v2); // BIKSSSKCB
}
