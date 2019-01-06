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
#include "RegManager.H"
#include "TimeManager.H"
#include "Interpolator.H"
#include "ParametrizationBase.H"
#include "ParamManager.H"

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
    
    Info<< "\nIIBR Lagrangian Solver\n";
   
    Info << "\nInitiliazing Registration Manager\n";
    CRegManager i_RegManager(mesh);
    
    Info << "\nInitializing Time Manager\n";
    CTimeManager i_TimeManager(mesh, 2);
    
    Info << "\nInitiliazing Interpolator\n";
    CInterpolator i_Interpolator(i_TimeManager, mesh);
    
    Info << "\nInitiliazing Parametrization Manager\n";
    CParamManager i_ParamManager(mesh, i_RegManager, "rootCase", i_Interpolator);
   
    #include "createFields.H"
    #include "postProcess.H"
    
    Info << "\nFinding root case path\n";
    
    //TODO Read root case
    instantList rootTimeDirs = runTime.findTimes("rootCase");
    
    label s = CMisc::FindClosestPastIndex(runTime.value(), rootTimeDirs);
    
    Info << "\nLoading First time step " << rootTimeDirs[s].value() << "\n";
    
    i_TimeManager.AdvanceEulerTime(rootTimeDirs[s].value());
    i_ParamManager.LoadEulerTimeStep(rootTimeDirs[s].value(), i_TimeManager.EulerTimeIndex(0));
    Info << "\nLoading second time step " << rootTimeDirs[s+1].value() << "\n";
    
    i_TimeManager.AdvanceEulerTime(rootTimeDirs[s+1].value());
    i_ParamManager.LoadEulerTimeStep(rootTimeDirs[s+1].value(), i_TimeManager.EulerTimeIndex(0));
    

    //Temporal interpolation for velocity field U before first step
    Info << "\nStarting time loop\n" << endl;
    while (runTime.loop())
    {
        scalar dCurrTime = runTime.time().value();
        
        Info<< "Time = " << runTime.timeName() << nl << endl;
        
        if (dCurrTime > i_TimeManager.EulerTime(i_TimeManager.EulerTimeIndex(0)))
        {
            s = CMisc::FindClosestPastIndex(runTime.value(), rootTimeDirs);
            const scalar dEulerTime = rootTimeDirs[s+1].value();
            i_TimeManager.AdvanceEulerTime(dEulerTime);
            i_ParamManager.LoadEulerTimeStep(dEulerTime, i_TimeManager.EulerTimeIndex(0));
        }
        
        i_Interpolator.UpdateFactors();
        
        U = i_Interpolator.TemporalInterpolate<volVectorField>(word("U"),label(0));
        Info << "\n Velocity field value at cell 10 is " << U[10] << "\n";
        
        Info<< "Evolving " << kinematicCloud.name() << endl;
        
        laminarTransport.correct();

        mu = laminarTransport.nu()*rhoInfValue;

        //Cloud evolves
        kinematicCloud.evolve();

        runTime.write();

        Info<< "ExecutionTime = " << runTime.elapsedCpuTime() << " s"
            << "  ClockTime = " << runTime.elapsedClockTime() << " s"
            << nl << endl;
    }

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
