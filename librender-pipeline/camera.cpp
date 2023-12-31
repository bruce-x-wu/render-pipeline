#include <assert.h>

#include "camera.hpp"

void Camera::set_eye_position(Point &eye_position)
{
  this->_eye_position = eye_position;
  this->_update_camera_transform();
}

void Camera::set_gaze_direction(Vector3 &gaze_direction)
{
  this->_gaze_direction = gaze_direction.normalize();
  this->_update_camera_transform();
}

void Camera::set_view_up_direction(Vector3 &view_up_direction)
{
  this->_view_up_direction = view_up_direction.normalize();
  this->_update_camera_transform();
}

void Camera::_update_camera_transform()
{
  const Vector3 w = -this->_gaze_direction.normalize();
  const Vector3 u = cross(this->_view_up_direction, w).normalize();
  const Vector3 v = cross(w, u);

  double rotation_transform_values[4][4] = {
    { u.get_x(), u.get_y(), u.get_z(), 0 },
    { v.get_x(), v.get_y(), v.get_z(), 0 },
    { w.get_x(), w.get_y(), w.get_z(), 0 },
    {         0,         0,         0, 1 },
  };
  Transform rotation_transform = Transform(rotation_transform_values);

  double translation_transform_values[4][4] = {
    { 1, 0, 0, -this->_eye_position.get_x() },
    { 0, 1, 0, -this->_eye_position.get_y() },
    { 0, 0, 1, -this->_eye_position.get_z() },
    { 0, 0, 0,                            1 },
  };
  Transform translation_transform = Transform(translation_transform_values);

  this->camera_transform = rotation_transform * translation_transform;
}

void OrthographicCamera::set_x_bounds(double left, double right)
{
  assert(left < right);
  this->_left = left;
  this->_right = right;
  this->_update_projection_transform();
  this->_update_view_volume_bounds();
}

void OrthographicCamera::set_y_bounds(double bottom, double top)
{
  assert(bottom < top);
  this->_bottom = bottom;
  this->_top = top;
  this->_update_projection_transform();
  this->_update_view_volume_bounds();
}

void OrthographicCamera::set_z_bounds(double near, double far)
{
  assert(near > far);
  this->_near = near;
  this->_far = far;
  this->_update_projection_transform();
  this->_update_view_volume_bounds();
}

void OrthographicCamera::_update_projection_transform()
{
  double projection_transform_values[4][4] = {
    { 2 / (this->_right - this->_left), 0, 0, -(this->_right + this->_left) / (this->_right - this->_left) },
    { 0, 2 / (this->_top - this->_bottom), 0, -(this->_top + this->_bottom) / (this->_top - this->_bottom) },
    { 0, 0, 2 / (this->_near - this->_far), -(this->_near + this->_far) / (this->_near - this->_far) },
    { 0, 0, 0, 1 },
  };
  this->projection_transform = Transform(projection_transform_values);
}

void OrthographicCamera::_update_view_volume_bounds()
{
  const Vector3 left_top_near_point = { this->_left, this->_top, this->_near };
  const Vector3 right_bottom_far_point = { this->_right, this->_bottom, this->_far };

  // left plane
  this->view_volume_bounds[0].normal = { -1, 0, 0 };
  this->view_volume_bounds[0].offset = -(dot(this->view_volume_bounds[0].normal, left_top_near_point));

  // right plane
  this->view_volume_bounds[1].normal = { 1, 0, 0 };
  this->view_volume_bounds[1].offset = -(dot(this->view_volume_bounds[1].normal, right_bottom_far_point));
  
  // top plane
  this->view_volume_bounds[2].normal = { 0, 1, 0 };
  this->view_volume_bounds[2].offset = -(dot(this->view_volume_bounds[2].normal, left_top_near_point));

  // bottom plane
  this->view_volume_bounds[3].normal = { 0, -1, 0 };
  this->view_volume_bounds[3].offset = -(dot(this->view_volume_bounds[3].normal, right_bottom_far_point));

  // near plane
  this->view_volume_bounds[4].normal = { 0, 0, 1 };
  this->view_volume_bounds[4].offset = -(dot(this->view_volume_bounds[4].normal, left_top_near_point));

  // far plane
  this->view_volume_bounds[5].normal = { 0, 0, -1 };
  this->view_volume_bounds[5].offset = -(dot(this->view_volume_bounds[5].normal, right_bottom_far_point));
}
