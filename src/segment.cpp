#include <segment.h>

double Segment::get_admittance () {
    // NOTE: i have no idea how accurate this is lol
    return area;
}

double Segment::get_impedance () {
    return 1 / get_admittance ();
}

void Segment::set_admittance (double admittance) {
    // NOTE: again... i have no idea how accurate this is lol
    area = admittance;
}

void Segment::set_impedance (double impedance) {
    set_admittance (1 / impedance);
}

double Segment::get () {
    return left + right;
}

double Segment::get_left () {
    return left;
}

double Segment::get_right () {
    return right;
}

void Segment::put (double left, double right) {
    _left += left;
    _right += right;
}

void Segment::put_direct (double left, double right) {
    this->left += left;
    this->right += right;
}

void Segment::flush () {
    left = _left;
    right = _right;
    _left = 0;
    _right = 0;
}

void Segment::set_gamma (double left, double right) {
    gamma_left = left;
    gamma_right = right;
}

double Segment::get_gamma_left () {
    return gamma_left;
}

double Segment::get_gamma_right () {
    return gamma_right;
}

void Segment::copy (Segment &source) {
    _left = source._left;
    _right = source._right;
    left = source.left;
    right = source.right;
}
