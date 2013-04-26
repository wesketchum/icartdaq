
#include "cetlib/exception.h"
#include "artdaq-demo/Compression/Accum.hh"

namespace demo {

  Accum::Accum(DataVec & out, SymTable const & syms)
    :
    syms_(syms),
    curr_word_(&*out.begin()),
    buf_end_(&*out.end()),
    curr_pos_(0),
    total_(0)
  {
    if (curr_word_ == buf_end_) {
      throw cet::exception("CompressionBufferSize")
          << "Provided buffer has zero size!";
    }
    fill(out.begin(), out.end(), 0);
  }

  void Accum::put(ADCCountVec::value_type const & val)
  {
    SymTable::value_type const & te = syms_[val];
    *curr_word_ |= (te.code_ << curr_pos_);
    curr_pos_ += te.bit_count_;
    total_ += te.bit_count_;
    if (curr_pos_ >= bits_per_word) {
      ++curr_word_;
      assert(curr_word_ != buf_end_); // Safety check.
      curr_pos_ = curr_pos_ - bits_per_word; // leftovers
      *curr_word_ = te.code_ >> (te.bit_count_ - curr_pos_);
    }
  }
}
