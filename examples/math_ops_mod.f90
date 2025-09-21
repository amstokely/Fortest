module math_ops_mod
   implicit none
   private
   public :: mat_mul, cross_product, normalize_vector

contains

   !> @brief Multiply two matrices (A * B).
   subroutine mat_mul(A, B, C)
      real, intent(in) :: A(:, :), B(:, :)
      real, intent(out) :: C(:, :)
      integer :: i, j, k
      C = 0.0
      do i = 1, size(A, 1)
         do j = 1, size(B, 2)
            do k = 1, size(A, 2)
               C(i, j) = C(i, j) + A(i, k) * B(k, j)
            end do
         end do
      end do
   end subroutine mat_mul

   !> @brief Compute the cross product of two 3D vectors.
   subroutine cross_product(u, v, w)
      real, intent(in)  :: u(3), v(3)
      real, intent(out) :: w(3)

      w(1) = u(2)*v(3) - u(3)*v(2)
      w(2) = u(3)*v(1) - u(1)*v(3)
      w(3) = u(1)*v(2) - u(2)*v(1)
   end subroutine cross_product

   !> @brief Normalize a 3D vector to unit length.
   subroutine normalize_vector(u, u_norm)
      real, intent(in)  :: u(3)
      real, intent(out) :: u_norm(3)
      real :: norm

      norm = sqrt(sum(u * u))

      if (norm > 0.0) then
         u_norm = u / norm
      else
         u_norm = 0.0
      end if
   end subroutine normalize_vector

end module math_ops_mod
