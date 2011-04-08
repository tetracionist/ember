#pragma once

#include "readConfig.h"

namespace BoundaryCondition {
    enum BC { FixedValue, ZeroGradient, ControlVolume, WallFlux, Floating };
}

class oneDimGrid
{
public:
    oneDimGrid();

    dvector x; // The grid points
    dvector dampVal; // ratio of convective to diffusive coefficients (e.g. nu/v)

    int alpha; // domain curvature parameter. 0: planar, 1: cylindrical
    size_t ju, jb; // indices of burned / unburned boundaries

    // Parameters for controlling internal grid points:
    double vtol_in;
    double dvtol_in;
    dvector vtol; // relative solution variable tolerance for point insertion
    dvector dvtol; // global derivative solution variable tolerance for point insertion
    double absvtol; // absolute tolerance (ignore components with range smaller than this)
    double rmTol; // relative grid point removal tolerance
    double uniformityTol; // maximum ratio of adjacent grid point separation distances
    double gridMin, gridMax; // minimum and maximum grid point separation
    double dampConst; // relative allowable numerical damping
    double centerGridMin; // minimum separation of center grid points for curved flames

    // Parameters for controlling exterior grid points:
    bool fixedBurnedVal;
    bool unburnedLeft;
    bool fixedLeftLoc;
    bool twinFlame;
    bool curvedFlame;

    double boundaryTol;
    double boundaryTolRm;
    double unstrainedDownstreamWidth;
    size_t addPointCount; // number of points to add when regridding

    // true for each component that counts for evaluating boundary grid point tolerances
    // Transport-eliminated species don't get to vote.
    vector<bool> leftComponents;
    vector<bool> rightComponents;

    // Derived mesh parameters (calculated by updateValues)
    dvector cfm, cf, cfp; // first centered difference
    dvector dlj;
    dvector hh;
    dvector rphalf;
    dvector r;
    size_t nVars; // number of variables at each grid point
    size_t nAdapt; // Only the first nAdapt variables at each point are used for adaptation
    size_t nPoints; // number of grid point
    size_t jj; // index of last grid point ( = nPoints-1)

    bool updated; // true if the grid changed on the last call to adapt or regrid

    // Update the grid based on the solutionState, and adjust it to fit
    // on the new grid. Each element of the solutionState is a vector
    // containing the value of a solution variable at each grid point.
    // Return value is true if the grid has been modified
    // adapt inserts and removes points within the problem domain
    // regrid inserts and removes points at the ends of the problem domain
    void adapt(vector<dvector>& y);
    void regrid(vector<dvector>& y);
    void regridUnstrained(vector<dvector>& y, dvector& qdot);
    void setOptions(const configOptions& options);
    void updateValues(void);
    void setSize(const size_t N);

    void updateBoundaryIndices(void);

    BoundaryCondition::BC leftBC;
    BoundaryCondition::BC rightBC;

private:
    void removePoint(int jRemove, vector<dvector>& y);
    void addPoint(int jInsert, vector<dvector>& y);

    bool removeRight(vector<dvector>& y);
    bool removeLeft(vector<dvector>& y);
    bool addRight(vector<dvector>& y);
    bool addLeft(vector<dvector>& y);
    bool addRightUnstrained(vector<dvector>& y, dvector& q);
    bool removeRightUnstrained(vector<dvector>& y, dvector& q);
};


class GridBased
{
public:
    GridBased();

    virtual void setGrid(const oneDimGrid& grid);

protected:
    // the grid:
    oneDimGrid grid;

    // local names for some things that are part of the grid:
    dvector& x;
    dvector& r;
    dvector& rphalf;
    dvector& hh;
    dvector& dlj;
    dvector& cfm;
    dvector& cf;
    dvector& cfp;
    int& alpha; //!< curved grid exponent. alpha = 1 for curved flames, 0 for planar flames.

    size_t& nPoints;
    size_t& jj;
};
