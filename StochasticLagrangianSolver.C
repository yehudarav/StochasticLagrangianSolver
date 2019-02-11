/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2016 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Application
    icoUncoupledKinematicParcelFoam

Description
    Transient solver for the of a single kinematic
    particle cloud.

    Uses a pre-calculated velocity field to evolve the cloud.

\*---------------------------------------------------------------------------*/
//-How do use geometric interpolation (interpolationCellPoint class)
//-Registry items are removed when out of scope
//-How to derive cloud and parcel




#include "fvCFD.H"
#include "singlePhaseTransportModel.H"
#include "turbulentTransportModel.H"
#include "basicStochasticCloud.H"
#include "ModelManager.H"

using namespace Foam;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //


int main(int argc, char *argv[])
{
    argList::addOption
    (
        "cloudName",
        "name",
        "specify alternative cloud name. default is 'kinematicCloud'"
    );

  
    //TODO Get root case from dictionary
    #include "setRootCase.H"
    #include "createTime.H"
    
    //TODO Read mesh from rootcase and not from curr case
    #include "createMesh.H"
    #include "createControl.H"

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
    
    Info << endl << "IIBR Lagrangian Solver" << endl;
    Info <<         "----------------------" << endl;
   
    Info << endl << "Initiliazing the Model Manager" << endl;
    CModelManager i_ModelManager(mesh,"rootCase", 2);
   
    #include "createFields.H"
    #include "postProcess.H"

/*
    Info << "\nStarting time loop\n" << endl;
    while (runTime.loop())
    {
    
        Info<< "Time = " << runTime.timeName() << nl << endl;
    	i_ModelManager.Advance(runTime.time().value());
        
        laminarTransport.correct();
        mu = laminarTransport.nu()*rhoInfValue;

        //Cloud evolves
        Info<< "Evolving " << kinematicCloud.name() << endl;
        //kinematicCloud.evolve();
        //runTime.write();

        Info<< "ExecutionTime = " << runTime.elapsedCpuTime() << " s"
            << "  ClockTime = " << runTime.elapsedClockTime() << " s"
            << nl << endl;
    }
*/
    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
