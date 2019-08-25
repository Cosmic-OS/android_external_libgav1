#ifndef LIBGAV1_SRC_UTILS_PARAMETER_TREE_H_
#define LIBGAV1_SRC_UTILS_PARAMETER_TREE_H_

#include <cassert>
#include <memory>

#include "src/utils/common.h"
#include "src/utils/constants.h"
#include "src/utils/memory.h"
#include "src/utils/types.h"

namespace libgav1 {

class ParameterTree : public Allocable {
 public:
  // Creates a parameter tree to store the parameters of a block of size
  // |block_size| starting at coordinates |row4x4| and |column4x4|. If |is_leaf|
  // is set to true, the memory will be allocated for the BlockParameters for
  // this node. Otherwise, no memory will be allocated. If |is_leaf| is set to
  // false, |block_size| must be a square block, i.e.,
  // kBlockWidthPixels[block_size] must be equal to
  // kBlockHeightPixels[block_size].
  ParameterTree(int row4x4, int column4x4, BlockSize block_size,
                bool is_leaf = false)
      : row4x4_(row4x4), column4x4_(column4x4), block_size_(block_size) {
    if (is_leaf) {
      SetPartitionType(kPartitionNone);
    }
  }

  // Move only (not Copyable).
  ParameterTree(ParameterTree&& other) = default;
  ParameterTree& operator=(ParameterTree&& other) = default;
  ParameterTree(const ParameterTree&) = delete;
  ParameterTree& operator=(const ParameterTree&) = delete;

  // Set the partition type of the current node to |partition|.
  // if (partition == kPartitionNone) {
  //   Memory will be allocated for the BlockParameters for this node.
  // } else if (partition != kPartitionSplit) {
  //   The appropriate child nodes will be populated and memory will be
  //   allocated for the BlockParameters of the children.
  // } else {
  //   The appropriate child nodes will be populated but they are considered to
  //   be hanging, i.e., future calls to SetPartitionType() on the child nodes
  //   will have to set them or their descendants to a terminal type.
  // }
  // This function must be called only once per node.
  void SetPartitionType(Partition partition);

  // Traverses the tree and searches for the node that contains the
  // BlockParameters for |row4x4| and |column4x4|. Returns nullptr, if the tree
  // does not contain the BlockParameters for the given coordinates.
  BlockParameters* Find(int row4x4, int column4x4) const;

  // Basic getters.
  int row4x4() const { return row4x4_; }
  int column4x4() const { return column4x4_; }
  BlockSize block_size() const { return block_size_; }
  Partition partition() const { return partition_; }
  ParameterTree* children(int index) const {
    assert(index < 4);
    return children_[index].get();
  }
  // Returns the BlockParameters object of the current node if one exists.
  // Otherwise returns nullptr. This function will return a valid
  // BlockParameters object only for leaf nodes.
  BlockParameters* parameters() const { return parameters_.get(); }

 private:
  Partition partition_ = kPartitionNone;
  std::unique_ptr<BlockParameters> parameters_ = nullptr;
  int row4x4_ = -1;
  int column4x4_ = -1;
  BlockSize block_size_ = kBlockInvalid;
  bool partition_type_set_ = false;

  // Child values are defined as follows for various partition types:
  //  * Horizontal: 0 top partition; 1 bottom partition; 2 nullptr; 3 nullptr;
  //  * Vertical: 0 left partition; 1 right partition; 2 nullptr; 3 nullptr;
  //  * Split: 0 top-left partition; 1 top-right partition; 2; bottom-left
  //    partition; 3 bottom-right partition;
  //  * HorizontalWithTopSplit: 0 top-left partition; 1 top-right partition; 2
  //    bottom partition; 3 nullptr;
  //  * HorizontalWithBottomSplit: 0 top partition; 1 bottom-left partition; 2
  //    bottom-right partition; 3 nullptr;
  //  * VerticalWithLeftSplit: 0 top-left partition; 1 bottom-left partition; 2
  //    right partition; 3 nullptr;
  //  * VerticalWithRightSplit: 0 left-partition; 1 top-right partition; 2
  //    bottom-right partition; 3 nullptr;
  //  * Horizontal4: 0 top partition; 1 second top partition; 2 third top
  //    partition; 3 bottom partition;
  //  * Vertical4: 0 left partition; 1 second left partition; 2 third left
  //    partition; 3 right partition;
  std::unique_ptr<ParameterTree> children_[4] = {};
};

}  // namespace libgav1

#endif  // LIBGAV1_SRC_UTILS_PARAMETER_TREE_H_
