// Algorithms and Operations
#include <algorithm>

// Assertions
#include <cassert>

// Data Structures
#include <set>
#include <vector>

// Files
#include <filesystem>
#include <fstream>
#include <istream>

// Other
#include <functional>
#include <utility>

// XML Parser
#include <pugixml.hpp>

// Common
#include "common/adapter.h"
#include "common/chrono.h"
#include "common/input.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// PARAMETER PARSING
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Path to input file.
////////////////////////////////////////////////////////////////////////////////////////////////////
std::string path = "";

////////////////////////////////////////////////////////////////////////////////////////////////////
class parsing_policy
{
public:
  static constexpr std::string_view name = "FLAM";
  static constexpr std::string_view args = "f:";

  static constexpr std::string_view help_text =
    "        -f PATH               Path to file containing a model\n"
    ;

  static inline bool
  parse_input(const int c, const char* arg)
  {
    switch (c) {
    case 'f': {
      if (!std::filesystem::exists(arg)) {
        std::cerr << "File '" << arg << "' does not exist\n";
        return true;
      }
      path = arg;
      return false;
    }
    default: return true;
    }
  }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// STATISTICS
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// LABEL
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief   Class to hold a security label pair.
///
/// \details Each security label consists of two components, `S` and `I`, where `S` is the
///          confidentiality and `I` is the integrity.
///
/// \remark  Based on A. Askarov's implementation in Troupe in the `Label` class.
////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename Adapter>
class label
{
public:
  using dd_type = typename Adapter::dd_t;

private:
  /// \brief Confidentiality
  dd_type confidentiality;

  /// \brief Integrity
  dd_type integrity;

  //////////////////////////////////////////////////////////////////////////////////////////////////
private:
  /// \brief Construct a DC Label
  label(const dd_type &c, const dd_type &i)
    : confidentiality(c)
    , integrity(i)
  {}

public:
  label() = delete;

  /// \brief Create label `⟨ci,ii⟩`
  label(Adapter &a, const int ci)
    : label(a, ci, ci)
  {}

  /// \brief Create label `⟨c,i⟩`
  label(Adapter &a, const int c, const int i)
    : label(a.ithvar(c), a.ithvar(i))
  {}

  /// \brief Most restrictive information flow.
  static label<Adapter>
  top(Adapter &a)
  {
    return label(a.bot(), a.top());
  }

  /// \brief Least restrictive information flow.
  static label<Adapter>
  bot(Adapter &a)
  {
    return label(a.top(), a.bot());
  }

  /// \brief Minimal authority.
  static label<Adapter>
  nil(Adapter &a)
  {
    return label(a.top(), a.top());
  }

  /// \brief Maximal authority.
  static label<Adapter>
  root(Adapter &a)
  {
    return label(a.bot(), a.bot());
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
public:
  /// \brief Whether information may flow from this label to `other`.
  ///
  /// \remark Information flows such that confidentiality gets more restrictive (you need more
  ///         permissions to read) and integrity is less restrictive (more people can write).
  bool flows_to(Adapter &a, label<Adapter> &other)
  {
    const bool c_constraint = a.is_imp(other.confidentiality, this.confidentiality);
    const bool i_constraint = a.is_imp(this.integrity, other.integrity);
    return c_constraint && i_constraint;
  }

  /// \brief Whether this label may act on behalf of `other`.
  ///
  /// \remark Authority flows such that both confidentiality and integrity (read and write
  ///         permissions) become more restrictive.
  bool acts_for(Adapter &a, label<Adapter> &other)
  {
    const bool c_constraint = a.is_imp(this.confidentiality, other.confidentiality);
    const bool i_constraint = a.is_imp(this.integrity, other.integrity);
    return c_constraint && i_constraint;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
public:
  /// \brief Join in the IFC lattice dimension, i.e. least upper bound.
  ///
  /// \details `L₁ ⊔ L₂ = ⟨S₁ ∧ S₂, I₁ ∨ I₂⟩`
  label
  join(Adapter &a, const label<Adapter> &other)
  {
    return label<Adapter>(a.apply_and(this.confidentiality, other.confidentiality),
                          a.apply_or(this.integrity, other.integrity));
  }

  /// \brief Meet in the IFC lattice, i.e. greatest lower bound.
  ///
  /// \details `L₁ ⊓ L2 = ⟨S₁ ∨ S₂, I₁ ∧ I₂⟩`
  label<Adapter>
  meet(Adapter &a, const label<Adapter> &other)
  {
    return label<Adapter>(a.apply_or(this.confidentiality, other.confidentiality),
                          a.apply_and(this.integrity, other.integrity));
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
public:
  /// \brief View of a label.
  ///
  /// \remark For more information, please read up on nonmalleable information flow control.
  label<Adapter>
  view(Adapter &a)
  {
    return label<Adapter>(this->integrity, a.top());
  }

  /// \brief Voice of a label.
  ///
  /// \remark For more information, please read up on nonmalleable information flow control.
  label<Adapter>
  voice(Adapter &a)
  {
    return label<Adapter>(a.top(), this->confidentiality);
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
public:
  /// \brief Obtain string representation of the label.
  std::string
  to_string(Adapter &a) const
  {
    std::stringstream ss;
    ss << "⟨ "
       << a.nodecount(this.confidentiality) << "|" << a.satcount(this.confidentiality)
       << " , "
       << a.nodecount(this.integrity)       << "|" << a.satcount(this.integrity)
       << " ⟩";
    return ss.str();
  }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// Execution
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename Adapter>
int
run_flam(int argc, char** argv)
{
  const bool should_exit = parse_input<parsing_policy>(argc, argv);
  if (should_exit) { return -1; }

  if (path == "") {
    std::cerr << "Input file not specified\n";
    return -1;
  }

  // Parse input file
  return run<Adapter>("flam", 2, [&](Adapter& adapter) {
    const label<Adapter> alice(adapter, 0);
    const label<Adapter> bob(adapter, 1);

    // TODO

    return 0;
  });
}
