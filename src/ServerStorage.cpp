#include "ServerStorage.h"
#include <stdexcept>
#include <string>

bool ServerStorage::is_initialized = false;
bool ServerStorage::is_capacity_set = false;

ServerStorage::ServerStorage() : buckets() {
  if (ServerStorage::is_initialized) {
    throw std::runtime_error("Only one ServerStorage can be used at a time in this implementation!");
  }
  ServerStorage::is_initialized = true;
}

void ServerStorage::setCapacity(int total_num_of_buckets) {
  if (this->is_capacity_set) {
    throw std::runtime_error("Capacity of ServerStorage cannot be changed");
  }
  this->is_capacity_set = true;
  this->capacity = total_num_of_buckets;
  this->buckets.assign(total_num_of_buckets, Bucket());
}

void ServerStorage::checkRWValid(int position) {
  if (!this->is_capacity_set) {
    throw std::runtime_error("Please call setCapacity before reading or writing any block");
  }
  if (position >= this->capacity || position < 0) {
    throw std::runtime_error("You are trying to access Bucket " + std::to_string(position) +
        ", but this Server contains only " + std::to_string(this->capacity) + " buckets.");
  }
}

Bucket ServerStorage::readBucket(int position) {
  this->checkRWValid(position);
  return this->buckets.at(position);
}

void ServerStorage::writeBucket(int position, const Bucket& bucket_to_write) {
  this->checkRWValid(position);
  this->buckets.at(position) = bucket_to_write;
}
