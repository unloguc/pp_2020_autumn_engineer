#include <vector>
#include <algorithm>
#include "allreduce_mpi.h"


#define MPI_TAG_REDUCE_VALUE 0xFF000000;
#define MPI_TAG_REDUCE_VALUE_MASK 0x00FFFFFF;

int getSequentialOperations(std::vector<int> vec, std::string ops) {
    const int  sz = vec.size();
    int reduction_elem = 0;
    if (ops == "+") {
        for (int i = 0; i < sz; i++) {
            reduction_elem += vec[i];
        }
    }
    else if (ops == "-") {
        for (int i = 0; i < sz; i++) {
            reduction_elem -= vec[i];
        }
    }
    else if (ops == "max") {
        reduction_elem = vec[0];
        for (int i = 1; i < sz; i++) {
            reduction_elem = std::max(reduction_elem, vec[i]);
        }
    }
    return reduction_elem;
}

int getParallelOperations(std::vector<int> global_vec,
    int count_size_vector, std::string ops) {
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    const int delta = count_size_vector / size;

#ifdef DEBUG_PRINT
    printf("[%d] Process: %d of %d, Vector size: %d, Operation: %s\n", rank, rank, size, count_size_vector, ops);
#endif


    if (rank == 0) {
        for (int proc = 1; proc < size; proc++) {
            MPI_Send(&global_vec[0] + proc * delta, delta,
                MPI_INT, proc, 0, MPI_COMM_WORLD);
        }
    }

    std::vector<int> local_vec(delta);
    if (rank == 0) {
        local_vec = std::vector<int>(global_vec.begin(),
            global_vec.begin() + delta);
    }
    else {
        MPI_Status status;
        MPI_Recv(&local_vec[0], delta, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }

    int global_sum = 0;
    int local_sum = getSequentialOperations(local_vec, ops);
    MPI_Op op_code;
    if (ops == "+" || ops == "-") { op_code = MPI_SUM; }
    if (ops == "max") { op_code = MPI_MAX; }
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, op_code, 0, MPI_COMM_WORLD);
    return global_sum;
}


int my_MPI_Reduce(void* send_data, void* recv_data, int count, MPI_Datatype datatype,
    MPI_Op op, int root, MPI_Comm comm) {

    int world_size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status status;

    int recv_value;
    switch (datatype) {
    case MPI_INT: 
        int *send_data_ = reinterpret_cast<int *>(send_data);
        int send_value;
        int recv_value;
        break;
    case MPI_FLOAT:
        float *send_data_ = reinterpret_cast<float *>(send_data);
        float send_value;
        float recv_value;
        break;
    case MPI_DOUBLE:
        double *send_data_ = reinterpret_cast<double *>(send_data);
        double send_value;
        double recv_value;
        break;
    default:
        MPI_Abort(MPI_COMM_WORLD, MPI_ERR_TYPE);
    }
    int datatype_size;
    MPI_Type_size(datatype, &datatype_size);
    char *recv_value = new char[datatype_size];
    int tag;
    double result = 0;
    for (size_t i = 0; i < count; i++) {
        if (rank == root) {
            MPI_Recv(&recv_value, 1, datatype, rank, tag, comm, &status);
            result += *(int *)recv_value;
        }
        else {
            int tag = i;
            tag |= MPI_TAG_REDUCE_VALUE;
            void *send_buf = (char *)send_data + i * datatype_size;
            MPI_Send(send_data, 1, datatype, root, tag, comm);
        }
    }

    return MPI_SUCCESS;
}


int my_MPI_Allreduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
    int world_size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int count_elements_per_process = std::max(1, world_size);
    int root = 0;
    
    //if (root == rank) {
    //    for (int i = root + 1; i < world_size; ++i) {
    //        MPI_Send(sendbuf + shift + partSize * i, partSize, MPI_DOUBLE, i, Tag, MPI_COMM_WORLD);
    //    }
    //}

    //// Get processes topology

    //// Send data to each process tree element
    //if (rank == root) {
    //    // If we are the root process, send our data to everyone
    //    int i;
    //    for (i = 0; i < world_size; i++) {
    //        if (i != rank) {
    //            MPI_Send(data, count, datatype, i, 0, comm);
    //        }
    //    }
    //}
    //else {
    //    // If we are a receiver process, receive the data from the root
    //    MPI_Recv(data, count, datatype, root, 0, communicator,
    //        MPI_STATUS_IGNORE);
    //}

    // Receive results from each process

    // Syncronize

    // Do required operation for root process

    // Send result to each process

    return MPI_SUCCESS;
}
