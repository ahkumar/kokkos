/*
//@HEADER
// ************************************************************************
//
//                        Kokkos v. 3.0
//       Copyright (2020) National Technology & Engineering
//               Solutions of Sandia, LLC (NTESS).
//
// Under the terms of Contract DE-NA0003525 with NTESS,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY NTESS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL NTESS OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Christian R. Trott (crtrott@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

// This file calls most of the basic Kokkos primitives. When combined with a
// testing library this tests that our shared-library loading based profiling
// mechanisms work

#include <iostream>
#include <Kokkos_Core.hpp>

int main() {
#ifdef KOKKOS_ENABLE_CXX11_DISPATCH_LAMBDA
  Kokkos::initialize();
  {
    using execution_space = Kokkos::DefaultExecutionSpace;
    using memory_space    = typename execution_space::memory_space;
    Kokkos::View<int*, memory_space> src_view("source", 10);
    Kokkos::View<int*, memory_space> dst_view("destination", 10);
    Kokkos::deep_copy(dst_view, src_view);
    Kokkos::parallel_for(
        "parallel_for", Kokkos::RangePolicy<execution_space>(0, 1),
        KOKKOS_LAMBDA(int i) { (void)i; });
    int result;
    Kokkos::parallel_reduce(
        "parallel_reduce", Kokkos::RangePolicy<execution_space>(0, 1),
        KOKKOS_LAMBDA(int i, int& hold_result) { hold_result += i; }, result);
    Kokkos::parallel_scan(
        "parallel_scan", Kokkos::RangePolicy<execution_space>(0, 1),
        KOKKOS_LAMBDA(const int i, int& hold_result, const bool final) {
          if (final) {
            hold_result += i;
          }
        });
    Kokkos::Profiling::pushRegion("push_region");
    Kokkos::Profiling::popRegion();
    uint32_t sectionId;
    Kokkos::Profiling::createProfileSection("created_section", &sectionId);
    Kokkos::Profiling::startSection(sectionId);
    Kokkos::Profiling::stopSection(sectionId);
    Kokkos::Profiling::destroyProfileSection(sectionId);
    Kokkos::Profiling::markEvent("profiling_event");
  }
  Kokkos::finalize();
#endif
}