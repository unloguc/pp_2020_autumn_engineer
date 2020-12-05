// Copyright 2020 Stoicheva Darya
#include "../../../modules/task_2/stoicheva_d_allreduce/allreduce_mpi.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <string>


template<typename result, typename source>
result pointer_cast(source *v) {
    return static_cast<result>(static_cast<void*>(v));
}


template<typename result, typename source>
result pointer_cast(const source *v) {
    return static_cast<result>(const_cast<void *>(static_cast<const void*>(v)));
}


void print_vector(const std::vector<int> vector, const size_t size, const std::string prefix,
    const std::string label, std::ostream &out) {
    print_vector<int>(vector, size, prefix, label, out);
}


void print_vector(const std::vector<float> vector, const size_t size, const std::string prefix,
    const std::string label, std::ostream &out) {
    print_vector<float>(vector, size, prefix, label, out);
}


void print_vector(const std::vector<double> vector, const size_t size, const std::string prefix,
    const std::string label, std::ostream &out) {
    print_vector<double>(vector, size, prefix, label, out);
}


template<typename T>
void print_vector(const std::vector<T> vector, const size_t size, const std::string prefix,
    const std::string label, std::ostream &out) {
#ifdef DEBUG_PRINT
    std::stringstream buffer;
    buffer << prefix + (label.empty() ? "Vector" : label) + ": {" << std::endl;
    buffer << prefix << "  ";
    for (size_t i = 0; i < size; i++) {
        buffer << vector[i] << ", ";
    }
    buffer << std::endl << prefix << "}" << std::endl;
    out << buffer.str();
#endif
}


template<typename T>
T print_vector2(const std::vector<T> vector, const size_t size, const std::string prefix,
    const std::string label, std::ostream &out) {
#ifdef DEBUG_PRINT
    std::stringstream buffer;
    buffer << prefix + (label.empty() ? "Vector" : label) + ": {" << std::endl;
    buffer << prefix << "  ";
    for (size_t i = 0; i < size; i++) {
        buffer << vector[i] << ", ";
    }
    buffer << std::endl << prefix << "}" << std::endl;
    out << buffer.str();
#endif
}


int swap_0_with_root(bool swap_required, int proc_id, int root_id) {
    int id = proc_id;
    if (swap_required) {
        if (proc_id == root_id)
            id = 0;
        if (proc_id == 0)
            id = root_id;
    }
    return id;
}


std::string value_to_string(const void *buffer, MPI_Datatype datatype, const int count = 1) {
    if (datatype == MPI_INT) {
        return std::to_string(*pointer_cast<int *>(buffer));
    } else if (datatype == MPI_FLOAT) {
        return std::to_string(*pointer_cast<float *>(buffer));
    } else if (datatype == MPI_DOUBLE) {
        return std::to_string(*pointer_cast<double *>(buffer));
    } else {
        return "Unsupported data type.";
    }
}


template<typename T>
T getSequentialOperations(const std::vector<T> vec, const MPI_Op op) {
    const int  sz = vec.size();
    T reduction_elem = 0;
    if (op == MPI_SUM) {
        for (int i = 0; i < sz; i++) {
            reduction_elem += vec[i];
        }
    } else if (op == MPI_PROD) {
        for (int i = 0; i < sz; i++) {
            reduction_elem *= vec[i];
        }
    } else if (op == MPI_MAX) {
        reduction_elem = vec[0];
        for (int i = 1; i < sz; i++) {
            reduction_elem = std::max(reduction_elem, vec[i]);
        }
    } else if (op == MPI_MIN) {
        reduction_elem = vec[0];
        for (int i = 1; i < sz; i++) {
            reduction_elem = std::min(reduction_elem, vec[i]);
        }
    }
    return reduction_elem;
}
template int getSequentialOperations<int>(const std::vector<int> vec, const MPI_Op op);
template float getSequentialOperations<float>(const std::vector<float> vec, const MPI_Op op);
template double getSequentialOperations<double>(const std::vector<double> vec, const MPI_Op op);


template<typename T>
int do_op_t(const size_t index, const void *in_buf, void *out_buf, MPI_Op op) {
    int status = MPI_SUCCESS;

    T *buffer = pointer_cast<T *>(out_buf);
    T *operand = pointer_cast<T *>(in_buf);
    // dbg printf("[00] index: %d  buffer: %s  operand: %s  op: ", int(index), to_string(buffer[index]).c_str(),
    // dbg        to_string(*operand).c_str());
    if (op == MPI_SUM) {
        buffer[index] += operand[index];
    } else if (op == MPI_MAX) {
        if (operand[index] > buffer[index]) {
            buffer[index] = operand[index];
        }
    } else if (op == MPI_MIN) {
        if (operand[index] < buffer[index]) {
            buffer[index] = operand[index];
        }
    } else if (op == MPI_PROD) {
        buffer[index] *= operand[index];
    } else {
        status = MPI_ERR_OP;
    }
    return status;
    }
    // dbg printf("  result buffer: %s  status: %d\n", to_string(buffer[index]).c_str(), status);


int do_op(const size_t index, const void *in_buf, void *out_buf, MPI_Datatype datatype, MPI_Op op) {
    int status;
    if (datatype == MPI_INT) {
        status = do_op_t<int>(index, in_buf, out_buf, op);
    } else if (datatype == MPI_FLOAT) {
        status = do_op_t<float>(index, in_buf, out_buf, op);
    } else if (datatype == MPI_DOUBLE) {
        status = do_op_t<double>(index, in_buf, out_buf, op);
    } else {
        status = MPI_ERR_TYPE;
    }
    return status;
}


template<typename T>
int my_MPI_Reduce_t(const void* sendbuf, void* recvbuf, int count, MPI_Datatype datatype,
    MPI_Op op, int root, MPI_Comm comm) {

    int world_size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int status = MPI_SUCCESS;

#ifdef DEBUG_PRINT
    printf("[%d] my_MPI_Reduce_Base(): count = %d\n", rank, count);
#endif

    if (root < 0 || root >= world_size) {
        return MPI_ERR_ROOT;
    }

    if (count == 0) {
        return status;
    }

    const int reduce_tag = 666;
    int last_power_of_2 = static_cast<int>(std::log2(world_size));

    T *send_data = pointer_cast<T *>(sendbuf);
    T *recv_data = pointer_cast<T *>(recvbuf);

    int datatype_size;
    MPI_Type_size(datatype, &datatype_size);
    T *recv_buf = static_cast<T *>(malloc(datatype_size * count));
    // T recv_buf[count];
    std::memcpy(recv_data, send_data, count * datatype_size);

    bool swap_0_with_root_required = root != 0;
    for (int step = last_power_of_2; step >= 0; step--) {
        int offset = 1 << step;
        int last_proc_in_step = std::min(world_size, 2 * offset) - 1;
        for (int proc = last_proc_in_step; proc >= 0; proc--) {
            int sender;
            int receiver;
            if (rank == proc) {
                if (rank + offset <= last_proc_in_step) {
                    // Acts as receiver
                    MPI_Status recv_status;
                    receiver = rank;
                    sender = rank + offset;
#ifdef DEBUG_PRINT
                    printf("[%d] RECEIVE:: Proc: %d,  Offset: %d,  Step: %d,  Sender process: %d,  "
                        "Receiver process %d\n", rank, proc, offset, step, sender, receiver);
                    fflush(stdout);
#endif
                    status = MPI_Recv(recv_buf, count, datatype,
                        swap_0_with_root(swap_0_with_root_required, sender, root),
                        reduce_tag, comm, &recv_status);
                    // status = MPI_Recv(recv_buf, count, datatype,
                    //     MPI_ANY_SOURCE,
                    //     MPI_ANY_TAG, comm, &recv_status);
#ifdef DEBUG_PRINT
                    sender = recv_status.MPI_SOURCE;
                    int tag = recv_status.MPI_TAG;
                    int error_code = recv_status.MPI_ERROR;
                    char error_string[MPI_MAX_ERROR_STRING];
                    int resultlen = 0;
                    MPI_Error_string(error_code, error_string, &resultlen);
                    error_string[resultlen] = 0;
                    printf("[%d] receiver process '%d' received data(%s) with tag(%d) from sender process '%d'  "
                        "current buffer: %s  (status: %s)\n",
                        rank, receiver, std::to_string(recv_buf[0]).c_str(), tag, sender,
                        std::to_string(recv_data[0]).c_str(), error_string);

                    print_vector(std::vector<T>(recv_buf, recv_buf + count), count, "[" + std::to_string(rank) +
                        "[", "recv_buf");
                    print_vector(std::vector<T>(recv_data, recv_data + count), count, "[" + std::to_string(rank) +
                        "[", "before operation");
                    fflush(stdout);
#endif

                    for (int i = 0; i < count; i++) {
                        status = do_op(i, recv_buf, recv_data, datatype, op);
                        if (status != MPI_SUCCESS) {
                            return status;
                        }
                    }

#ifdef DEBUG_PRINT
                    print_vector(std::vector<T>(recv_data, recv_data + count), count, "[" + std::to_string(rank) +
                        "[", "after operation");
                    fflush(stdout);
#endif
                }
                if (rank - offset >= 0) {
                    // Acts as sender
                    receiver = rank - offset;
                    sender = rank;

#ifdef DEBUG_PRINT
                    printf("[%d] SEND:: Proc: %d,  Offset: %d,  Step: %d,  Sender process: %d,  Receiver "
                        "process %d\n", rank, proc, offset, step, sender, receiver);
                    fflush(stdout);
#endif

                    MPI_Send(recv_data, count, datatype, swap_0_with_root(swap_0_with_root_required, receiver, root),
                        reduce_tag, comm);

#ifdef DEBUG_PRINT
                    printf("[%d] Sender process '%d' sent data(%s) with tag(%d) to receiver process '%d'\n",
                        rank, sender, std::to_string(recv_data[0]).c_str(), reduce_tag, receiver);
                    fflush(stdout);
#endif
                }
            }
        }
    }
    free(recv_buf);
    return status;
}

int my_MPI_Bcast(void *buf, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {
    int status = MPI_SUCCESS;

    int world_size, rank;
    MPI_Comm_size(comm, &world_size);
    MPI_Comm_rank(comm, &rank);

    if (root < 0 || root >= world_size) {
        return MPI_ERR_ROOT;
    }

    if (rank == root) {
        for (int proc = 0; proc < world_size; proc++) {
            if (proc != root) {
                MPI_Send(buf, count, datatype, proc, 0, comm);
            }
        }
    } else {
        MPI_Status recv_status;
        status = MPI_Recv(buf, count, datatype, root, 0, comm, &recv_status);
    }

    return status;
}


int my_MPI_Bcast_Tree(void *buf, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {
    int status = MPI_SUCCESS;
    const int bcast_tag = 112;

    int world_size, rank;
    MPI_Comm_size(comm, &world_size);
    MPI_Comm_rank(comm, &rank);

    int last_power_of_2 = static_cast<int>(std::log2(world_size));

    if (root < 0 || root >= world_size) {
        return MPI_ERR_ROOT;
    }

    // Need to place non zero root on top of broadcasting tree
    bool swap_0_with_root_required = root != 0;

    // Broadcasting (S - sender, R - receiver):
    // 1. S0 -> R1
    // 2. S0 -> R2, S1 -> R3
    // 3. S0 -> R4, S1 -> R5, S2 -> R6, S3 -> R7
    // ...
    for (int step = 0; step <= last_power_of_2; step++) {
        int offset = 1 << step;
        for (int proc = 0; proc < offset; proc++) {
            int sender = proc;
            int receiver = proc + offset;
            if (rank == sender || rank == receiver) {
#ifdef DEBUG_PRINT
                printf("[%d] BCAST - Step: %d,  Offset: %d,  Proc: %d,  Sender: %d, Receiver: %d\n",
                    rank, step, offset, proc, sender, receiver);
                fflush(stdout);
#endif
                if (rank == receiver && sender >= 0) {
                    MPI_Status recv_status;
#ifdef DEBUG_PRINT
                    printf("[%d] Receiver process '%d' waiting data with tag(%d) from sender process '%d'"
                        "  current buffer: %s\n", rank, receiver, bcast_tag, sender,
                        value_to_string(buf, datatype).c_str());
                    fflush(stdout);
#endif
                    status = MPI_Recv(buf, count, datatype,
                        swap_0_with_root(swap_0_with_root_required, sender, root),
                        bcast_tag, comm, &recv_status);
#ifdef DEBUG_PRINT
                    int sender = recv_status.MPI_SOURCE;
                    int tag = recv_status.MPI_TAG;
                    int error_code = recv_status.MPI_ERROR;
                    char error_string[MPI_MAX_ERROR_STRING];
                    int resultlen = 0;
                    MPI_Error_string(error_code, error_string, &resultlen);
                    error_string[resultlen] = 0;
                    printf("[%d] receiver process '%d' received data(%s) with tag(%d) from sender process '%d'"
                        "  (status: %s)\n", rank, receiver, value_to_string(buf, datatype).c_str(), tag, sender,
                        error_string);
                    fflush(stdout);
#endif
                }
                if (rank == sender && receiver < world_size) {
                    MPI_Send(buf, count, datatype, swap_0_with_root(swap_0_with_root_required, receiver, root),
                        bcast_tag, comm);
#ifdef DEBUG_PRINT
                    printf("[%d] Sender process '%d' sent data(%s) with tag(%d) to receiver process '%d'\n", rank,
                        sender, value_to_string(buf, datatype).c_str(), bcast_tag, receiver);
                    fflush(stdout);
#endif
                }
            }
        }
    }

    return status;
}

int my_MPI_Allreduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
    int status = MPI_SUCCESS;

    int world_size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int root = 0;

    if (datatype == MPI_INT) {
        status = my_MPI_Reduce_t<int>(sendbuf, recvbuf, count, datatype, op, root, comm);
    } else if (datatype == MPI_FLOAT) {
        status = my_MPI_Reduce_t<float>(sendbuf, recvbuf, count, datatype, op, root, comm);
    } else if (datatype == MPI_DOUBLE) {
        status = my_MPI_Reduce_t<double>(sendbuf, recvbuf, count, datatype, op, root, comm);
    } else {
        status = MPI_ERR_TYPE;
    }
    MPI_Barrier(comm);
    my_MPI_Bcast_Tree(recvbuf, count, datatype, root, comm);
    return status;
}
