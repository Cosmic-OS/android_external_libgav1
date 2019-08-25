#ifndef LIBGAV1_SRC_UTILS_BLOCK_PARAMETERS_HOLDER_H_
#define LIBGAV1_SRC_UTILS_BLOCK_PARAMETERS_HOLDER_H_

#include <memory>

#include "src/utils/array_2d.h"
#include "src/utils/constants.h"
#include "src/utils/parameter_tree.h"
#include "src/utils/types.h"

namespace libgav1 {

// Holds a 2D array of |ParameterTree| objects. Each tree stores the parameters
// corresponding to a superblock.
class BlockParametersHolder {
 public:
  // If |use_128x128_superblock| is true, 128x128 superblocks will be used,
  // otherwise 64x64 superblocks will be used.
  BlockParametersHolder(int rows4x4, int columns4x4,
                        bool use_128x128_superblock);

  // Not copyable or movable.
  BlockParametersHolder(const BlockParametersHolder&) = delete;
  BlockParametersHolder& operator=(const BlockParametersHolder&) = delete;

  // Finds the BlockParameters corresponding to |row4x4| and |column4x4|. This
  // is done as a simple look up of the |block_parameters_cache_| matrix.
  // Returns nullptr if the BlockParameters cannot be found.
  BlockParameters* Find(int row4x4, int column4x4) {
    return block_parameters_cache_[row4x4][column4x4];
  }

  // Returns the ParameterTree corresponding to superblock starting at (|row|,
  // |column|).
  ParameterTree* Tree(int row, int column) { return trees_[row][column].get(); }

  // Fills the cache matrix for the block starting at |row4x4|, |column4x4| of
  // size |block_size| with the pointer |bp|.
  void FillCache(int row4x4, int column4x4, BlockSize block_size,
                 BlockParameters* bp);

 private:
  int rows4x4_;
  int columns4x4_;
  Array2D<std::unique_ptr<ParameterTree>> trees_;

  // This is a 2d array of size |rows4x4_| * |columns4x4_|.This is filled in by
  // FillCache() and used by Find() to perform look ups using exactly one look
  // up (instead of traversing the entire tree).
  Array2D<BlockParameters*> block_parameters_cache_;
};

}  // namespace libgav1

#endif  // LIBGAV1_SRC_UTILS_BLOCK_PARAMETERS_HOLDER_H_
