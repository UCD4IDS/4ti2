#include <cstring>
#include <iostream>
#include "4ti2/4ti2.h"
#include "groebner/QSolveAlgorithm.h"
#include "groebner/VectorArrayAPI.h"
#include "groebner/VectorArrayStream.h"
#include "groebner/LatticeBasis.h"
#include "groebner/CircuitsAPI.h"

using namespace _4ti2_;

CircuitsAPI::CircuitsAPI()
{
}

CircuitsAPI::~CircuitsAPI()
{
}

void
CircuitsAPI::compute()
{
    // Consistency and default value checking.
    if (!matrix && !lat) {
        std::cerr << "ERROR: No matrix specified.\n";
        exit(1);
    }
    if (!matrix) {
        matrix = new VectorArrayAPI(0, lat->get_num_cols());
        lattice_basis(lat->data, matrix->data);
    }
    if (!lat) {
        lat = new VectorArrayAPI(0, matrix->get_num_cols());
        lattice_basis(matrix->data, lat->data);
    }
    if (!sign) {
        sign = new VectorArrayAPI(1, matrix->get_num_cols());
        for (Index i = 0; i < sign->get_num_cols(); ++i) { sign->data[0][i] = 2; }
    }
    if (!rel) {
        rel = new VectorArrayAPI(1, matrix->get_num_cols());
        for (Index i = 0; i < rel->get_num_cols(); ++i) { rel->data[0][i] = 0; }
    }
    assert(sign->get_number() == 1);
    assert(matrix->get_num_cols() == sign->get_num_cols());

    std::cout << "Matrix:\n";
    matrix->write(std::cout);
    std::cout << "Sign:\n";
    sign->write(std::cout);
    std::cout << "Rel:\n";
    rel->write(std::cout);
    std::cout << "Lat:\n";
    lat->write(std::cout);

    // Delete previous computation.
    delete ray; delete cir; delete qfree;
    ray = new VectorArrayAPI(0, matrix->get_num_cols());
    ray->data.insert(lat->data);
    cir = new VectorArrayAPI(0, matrix->get_num_cols());
    qfree = new VectorArrayAPI(0, matrix->get_num_cols());

    QSolveAlgorithm alg(algorithm, order);
    alg.compute(matrix->data, ray->data, cir->data, qfree->data, rel->data[0], sign->data[0]); 

    ray->data.sort();
    cir->data.sort();
    qfree->data.sort();
    VectorArray::transfer(ray->data, 0, ray->data.get_number(), cir->data, 0);
}

void
CircuitsAPI::write_usage()
{
    std::cerr << "Usage: circuits [options] <PROJECT>\n\n";
    std::cerr << "Computes the circuits of a cone.\n";
    write_input_files();
    write_output_files();
    write_options();
}

void
CircuitsAPI::write_output_files()
{
    std::cerr << "\
Output Files:\n\
  PROJECT.cir         The circuits of the cone.\n\
  PROJECT.qfree       A basis for the linear subspace of the cone.\n\
                      If this file does not exist then the linear subspace \n\
                      is trivial.\n\n";
}

void
CircuitsAPI::write(const char* basename_c_str)
{
    std::string basename(basename_c_str);

    std::string cir_filename(basename + ".cir");
    cir->write(cir_filename.c_str());

    std::string qfree_filename(basename + ".qfree");
    qfree->write(qfree_filename.c_str());
}