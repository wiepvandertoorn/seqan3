#include <vector>
#include <seqan3/io/stream/debug_stream.hpp>
#include <seqan3/search/fm_index/all.hpp>

using namespace seqan3;

int main()
{

// TODO(comments): outputs [A, A, G] instead of AAG

{
debug_stream << "Example extend_left(seq)\n";
//! [extend_left_seq]
std::vector<dna4> genome{"GAATTAATGAAC"_dna4};
bi_fm_index index{genome}; // build the index

auto cur = index.begin();         // create an cursor
cur.extend_right("AAC"_dna4);     // search the sequence "AAC"
debug_stream << cur.query() << '\n'; // outputs "AAC"
cur.extend_left("ATG"_dna4);      // extend the query to "ATGAAT"
                                 // The rightmost character of "ATG" is extended to the left first.
debug_stream << cur.query() << '\n'; // outputs "ATGAAT"
//! [extend_left_seq]
}

{
debug_stream << "Example cycle_back() and cycle_front()\n";
//! [cycle]
std::vector<dna4> genome{"GAATTAATGAAC"_dna4};
bi_fm_index index{genome};           // build the index

auto cur = index.begin();             // create an cursor
// cur.cycle_back();                  // cycle_back / cycle_front on begin() is undefined behaviour!
cur.extend_right("AAC"_dna4);         // search the sequence "AAC"
debug_stream << cur.query() << '\n';     // outputs "AAC"
debug_stream << cur.last_char() << '\n'; // outputs 'C'

// cur.cycle_front();                 // undefined behaviour! only cycle_back() is allowed after extend_right()
cur.cycle_back();                     // search the sequence "AAT"
debug_stream << cur.query() << '\n';     // outputs "AAT"
debug_stream << cur.last_char() << '\n'; // outputs 'T'

cur.extend_left('G'_dna4);             // search the sequence "GAAT"
debug_stream << cur.query() << '\n';     // outputs "GAAC"
debug_stream << cur.last_char() << '\n'; // outputs 'G'

// cur.cycle_back();                  // undefined behaviour! only cycle_front() is allowed after extend_left()
cur.cycle_front();                    // search the sequence "TAAT"
debug_stream << cur.query() << '\n';     // outputs "TAAT"
debug_stream << cur.last_char() << '\n'; // outputs 'T'

cur.cycle_front();                    // search the sequence "TAAT"
debug_stream << cur.query() << '\n';     // outputs "TAAT"
debug_stream << cur.last_char() << '\n'; // outputs 'T'
//! [cycle]
}

{
debug_stream << "Example to_fwd_cursor()\n";
//! [to_fwd_cursor]
std::vector<dna4> genome{"GAATTAACGAAC"_dna4};
bi_fm_index index{genome};               // build the index

auto cur = index.begin();                 // create an cursor
cur.extend_left("AAC"_dna4);              // search the sequence "AAC"
debug_stream << cur.query() << '\n';         // outputs "AAC"
auto uni_cur = cur.to_fwd_cursor();      // unidirectional cursor on the text "GAATTAACGAAC"
debug_stream << uni_cur.query() << '\n';     // outputs "CAA"
// Undefined behaviour! Cannot be called on the forward cursor if the last extension on the bidirectional
// cursor was to the left:
// cur.cycle_back();
// debug_stream << cur.last_char() << '\n';

uni_cur.extend_right('G'_dna4);            // search the sequence "AACG"
debug_stream << uni_cur.query() << '\n';     // outputs "AACG"
debug_stream << uni_cur.last_char() << '\n'; // outputs 'G'
uni_cur.cycle_back();                     // returns false since there is no sequence "AACT" in the text.
//! [to_fwd_cursor]
}

{
debug_stream << "Example to_rev_cursor()\n";
//! [to_rev_cursor]
std::vector<dna4> genome{"GAATTAACGAAC"_dna4};
bi_fm_index index{genome};               // build the index

auto cur = index.begin();                 // create an cursor
cur.extend_right("AAC"_dna4);             // search the sequence "AAC"
debug_stream << cur.query() << '\n';         // outputs "AAC"
auto uni_cur = cur.to_rev_cursor();      // unidirectional cursor on the text "CAAGCAATTAAG"
debug_stream << uni_cur.query() << '\n';     // outputs "CAA"
// Undefined behaviour! Cannot be called on the reversed cursor if the last extension on the bidirectional
// cursor was to the right:
// cur.cycle_back();
// debug_stream << cur.last_char() << '\n';

uni_cur.extend_right('G'_dna4);            // search the sequence "CAAG"
debug_stream << uni_cur.query() << '\n';     // outputs "CAAG"
debug_stream << uni_cur.last_char() << '\n'; // outputs 'G'
uni_cur.cycle_back();                     // search the sequence "CAAT"
//! [to_rev_cursor]
}

    return 0;
}
