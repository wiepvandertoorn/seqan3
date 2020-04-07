#include <vector>

#include <seqan3/alphabet/nucleotide/dna4.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/search/fm_index/bi_fm_index.hpp>

int main()
{
    using seqan3::operator""_dna4;

    seqan3::debug_stream << "Example cycle_back() and cycle_front()\n";

    std::vector<seqan3::dna4> genome{"GAATTAATGAAC"_dna4};
    seqan3::bi_fm_index index{genome};                      // build the bidirectional index

    auto cur = index.cursor();                              // create a cursor
    // cur.cycle_back();                                    // cycle_back / cycle_front on begin() is undefined behaviour!
    cur.extend_right("AAC"_dna4);                           // search the sequence "AAC"
    seqan3::debug_stream << cur.path_label(genome) << '\n'; // outputs "AAC"
    seqan3::debug_stream << cur.last_rank() << '\n';        // outputs 1

    // cur.cycle_front();                                   // undefined behaviour! only cycle_back() is allowed after extend_right()
    cur.cycle_back();                                       // search the sequence "AAT"
    seqan3::debug_stream << cur.path_label(genome) << '\n'; // outputs "AAT"
    seqan3::debug_stream << cur.last_rank() << '\n';        // outputs 3

    cur.extend_left('G'_dna4);                              // search the sequence "GAAT"
    seqan3::debug_stream << cur.path_label(genome) << '\n'; // outputs "GAAC"
    seqan3::debug_stream << cur.last_rank() << '\n';        // outputs 2

    // cur.cycle_back();                                    // undefined behaviour! only cycle_front() is allowed after extend_left()
    cur.cycle_front();                                      // search the sequence "TAAT"
    seqan3::debug_stream << cur.path_label(genome) << '\n'; // outputs "TAAT"
    seqan3::debug_stream << cur.last_rank() << '\n';        // outputs 3

    cur.cycle_front();                                      // search the sequence "TAAT"
    seqan3::debug_stream << cur.path_label(genome) << '\n'; // outputs "TAAT"
    seqan3::debug_stream << cur.last_rank() << '\n';        // outputs 3
}
