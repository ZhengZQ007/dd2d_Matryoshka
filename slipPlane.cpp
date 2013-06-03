/**
 * @file slipPlane.cpp
 * @author Adhish Majumdar
 * @version 0.0
 * @date 03/06/2013
 * @brief Definition of the member functions of the SlipPlane class.
 * @details This file defines the member functions of the SlipPlane class.
 */

#include "slipPlane.h"

// Constructors
/**
 * @brief Default constructor.
 * @details The slip plane is initialized with default parameters specified in the file slipPlaneDefaults.h.
 */
SlipPlane::SlipPlane ()
{
  // Initialize the default variables.
  Vector3d pos(DEFAULT_SLIPPLANE_POSITION_0,
	       DEFAULT_SLIPPLANE_POSITION_1,
	       DEFAULT_SLIPPLANE_POSITION_2);
  Vector3d normal(DEFAULT_SLIPPLANE_NORMALVECTOR_0,
		  DEFAULT_SLIPPLANE_NORMALVECTOR_1,
		  DEFAULT_SLIPPLANE_NORMALVECTOR_2);
  Vector3d ends[2];
  ends[0] = Vector3d(DEFAULT_SLIPPLANE_EXTREMITY1_0,
		     DEFAULT_SLIPPLANE_EXTREMITY1_1,
		     DEFAULT_SLIPPLANE_EXTREMITY1_2);
  ends[1] = Vector3d(DEFAULT_SLIPPLANE_EXTREMITY2_0,
		     DEFAULT_SLIPPLANE_EXTREMITY2_1,
		     DEFAULT_SLIPPLANE_EXTREMITY2_2);
  std::vector<Dislocation> dislocationList(1, Dislocation());
  std::vector<DislocationSource> dislocationSourceList(1, DislocationSource());
  
  *this = SlipPlane(ends, normal, pos, dislocationList, dislocationSourceList);
}

/**
 * @brief Constructor that specifies all members explicitly.
 * @details The slip plane is initialized with parameters specified in the arguments.
 * @param ends Pointer to an array of type Vector3d, containing the position vectors of the extremities of the slip plane in consecutive locations.
 * @param normal The normal vector of the slip plane.
 * @param pos The position vector of the slip plane. (This parameter is useful for locating the slip plane within a slip system)
 * @param dislocationList A vector container of type Dislocation containing the dislocations lying on this slip plane.
 * @param dislocationSourceList A vector container of type DislocationSource containing the dislocation sources lying on this slip plane.
 */
SlipPlane::SlipPlane (Vector3d *ends, Vector3d normal, Vector3d pos, std::vector<Dislocation> dislocationList, std::vector<DislocationSource> dislocationSourceList)
{
  this->setExtremities (ends);
  this->setNormal (normal);
  this->setPosition (pos);
  this->setDislocationList (dislocationList);
  this->setDislocationSourceList (dislocationSourceList);

  // Fill the velocity vector with zero vectors
  int nDisl = this->getNumDislocations ();
  this->dislocationStresses.resize(nDisl, Stress ());
  this->dislocationVelocities.resize(nDisl, Vector3d());
  this->dislocationForces.resize(nDisl, Vector3d());
    
  this->calculateRotationMatrix ();
}

// Assignment functions
/**
 * @brief Set the extremities of the slip plane.
 * @param ends Pointer to an array of type Vector3d, containing the position vectors of the extremities of the slip plane in consecutive locations.
 */
void SlipPlane::setExtremities (Vector3d *ends)
{
  this->extremities[0] = *(ends);
  this->extremities[1] = *(ends+1);
}

/**
 * @brief Set the normal vector of the slip plane.
 * @param normal The normal vector of the slip plane.
 */
void SlipPlane::setNormal (Vector3d normal)
{
  this->normalVector = normal;
}

/**
 * @brief Set the position of the slip plane.
 * @param pos The position vector of the slip plane. (This parameter is useful for locating the slip plane within a slip system)
 */
void SlipPlane::setPosition (Vector3d pos)
{
  this->position = pos;
}

/**
 * @brief Set the list of dislocations of the slip plane.
 * @param dislocationList A vector container of type Dislocation containing the dislocations lying on this slip plane.
 */
void SlipPlane::setDislocationList (std::vector<Dislocation> dislocationList)
{
  this->dislocations = dislocationList;
}

/**
 * @brief Set the list of dislocation sources on the slip plane.
 * @param dislocationSourceList A vector container of type DislocationSource containing the dislocation sources lying on this slip plane.
 */
void SlipPlane::setDislocationSourceList (std::vector<DislocationSource> dislocationSourceList)
{
  this->dislocationSources = dislocationSourceList;
}

// Access functions
/**
 * @brief Get the position vector of the extremity whose index is provided as argument.
 * @param i Index of the extremity. Possible values: 0, 1
 * @return Position vector of the extremity indicated by the argument, returned as a variable of type Vector3d.
 */
Vector3d SlipPlane::getExtremity (int i)
{
  if (i==0 || i==1)
  {
    return (this->extremities[i];
  }
  else
  {
    return (Vector3d());
  }
}

/**
 * @brief Get the position vectors of the extremities of the slip plane.
 * @return Pointer to an array containing the position vectors of the two extremities of the slip plane, variables of type Vector3d.
 */
Vector3d* SlipPlane::getExtremities ()
{
  return (this->extremities);
}

/**
 * @brief Get the normal vector of the slip plane.
 * @return The normal vector of the slip plane, in a variable of type Vector3d.
 */
Vector3d SlipPlane::getNormal ()
{
  return (this->normalVector);
}

/**
 * @brief Get the position vector of the slip plane.
 * @details This function returns the position vector of the slip plane. The position vector is redundant because the slip plane is completely defined by its extremities and the normal vector. Nevertheless, this value can be useful to locate the slip plane within a slip system.
 * @return Position vector of the slip plane, in a variable of type Vector3d.
 */
Vector3d SlipPlane::getPosition ()
{
  return (this->position);
}

/**
 * @brief Get the dislocation on the slip plane indicated by the index provided as argument.
 * @details The slip plane contains several dislocations that are stored in a vector container. This function returns the dislocation in that vector that corresponds to the index provided as argument.
 * @param i Index of the required dislocation in the vector. This value should be greater than or equal to 0 and less than the number of dislocations on the slip plane.
 * @param d Pointer to the memory location where the required dislocation is to be stored. Space in memory must be pre-allocated.
 * @return True if the provided index is greater than or equal to 0 and less than the number of dislocations on the slip plane (the memory location pointed to by d is populated with the Dislocation data). Otherwise, the return value is false.
 */
bool SlipPlane::getDislocation (int i, Dislocation* d)
{
  if (i>=0 && i<this->dislocations.size ())
  {
    *d = this->dislocations[i];
    return (true);
  }
  else
  {
    return (false);
  }
}

/**
 * @brief Get the entire vector container which holds the dislocations lying on this slip plane.
 * @return The vector of dislocations lying on this slip plane.
 */
std::vector<Dislocation> SlipPlane::getDislocationList ()
{
  return (this->dislocations);
}

/**
 * @brief Get the number of dislocations.
 * @return The number of dislocations on the slip plane.
 */
 int SlipPlane::getNumDislocations ()
 {
   return (this->dislocations.size ());
 }

/**
 * @brief Get the dislocation source on the slip plane indicated by the index provided as argument.
 * @details The slip plane contains several dislocation sources that are stored in a vector container. This function returns the dislocation source in that vector that corresponds to the index provided as argument.
 * @param i Index of the required dislocation source in the vector. This value should be greater than or equal to 0 and less than the number of dislocation sources on the slip plane.
 * @param dSource Pointer to the memory location where the required dislocation source is to be stored. Space in memory must be pre-allocated.
 * @return True if the provided index is greater than or equal to 0 and less than the number of dislocation sources on the slip plane (the memory location pointed to by d is populated with the DislocationSource data). Otherwise, the return value is false.
 */
bool SlipPlane::getDislocationSource (int i, DislocationSource* dSource)
{
  if (i>=0 && i<this->dislocationSources.size ())
  {
    *dSource = this->dislocationSources[i];
    return (true);
  }
  else
  {
    return (false);
  }
}

/**
 * @brief Get the entire vector container which holds the dislocation sources lying on this slip plane.
 * @return The vector of dislocation sources lying on this slip plane.
 */
std::vector<DislocationSource> SlipPlane::getDislocationSourceList ()
{
  return (this->dislocationSources);
}

/**
 * @brief Get the number of dislocation sources.
 * @return The number of dislocation sources on the slip plane.
 */
 int SlipPlane::getNumDislocationSources ()
 {
   return (this->dislocationSources.size ());
 }

/**
 * @brief Get the rotation matrix for this slip plane.
 * @return The rotation matrix of this slip plane, in a variable of type RotationMatrix.
 */
RotationMatrix SlipPlane::getRotationMatrix ()
{
  return (this->rotationMatrix);
}

/**
 * @brief Get the axis (expressed in the global co-ordinate system) of the slip plane's local co-ordinate system, as indicated by the argument. (0, 1, 2)=(x, y, z).
 * @param i Index of the axis that is to be returned. (0, 1, 2)=(x, y, z).
 * @return The desired axis of the slip plane's local co-ordinate system, expressed in the global co-ordinate system. In case of invalid argument, a zero vector is returned.
 */
Vector3d SlipPlane::getAxis (int i)
{
  Vector3d = axis;
  
  if (i==2)
  {
    // Z-axis
    axis = this->normalVector;
  }
  
  if (i==0)
  {
    // X-axis
    axis = (this->extremities[1] - this->extremities[0]);
  }
  
  if (i==1)
  {
    // Y-axis = Z x X
    axis = this->getAxis(2) ^ this->getAxis(0);
  }
  
  return ( axis.normalize() );
}     

// Operations
/**
 * @brief Calculates the rotation matrix for this slip plane.
 * @details The slip plane has a local co-ordinate system whose axes are the following: z-axis||normal vector and x-axis||slip plane vector (vector joining the extremities). The rotation matrix is calculated in order to carry out transformations between the global and local co-ordinate systems.
 */
void SlipPlane::calculateRotationMatrix ()
{
  Vector3d *unPrimed = new Vector3d[3];	// Old system (global)
  Vector3d *primed   = new Vector3d[3];	// New system (local)
    
  int i, j;
  
  // Prepare the global and local systems
  for (i=0; i<3; i++)
  {
    for (j=0; j<3; j++)
    {
      unPrimed[i].setValue(j, (double)(i==j));
    }
    
    primed[i] = this->getAxis(i);
  }
  
  // Calculate the rotationMatrix
  this->rotationMatrix = RotationMatrix(unPrimed, primed);
  
  // Free memory
  delete(unPrimed);	unPrimed = NULL;
  delete(primed);	primed = NULL;
}

/**
 * @brief Calculates the total stress field experienced by each dislocation and stored it in the STL vector container dislocationStresses.
 * @details The total stress field is calculated as a superposition of the applied stress field and the stress fields experienced by each dislocation due to every other dislocation in the simulation.
 * @param appliedStress The stress applied externally.
 * @param mu Shear modulus of the material.
 * @param nu Poisson's ratio.
 */
 void SlipPlane::calculateDislocationStresses (Stress appliedStress, double mu, double nu)
 {
   std::vector<Dislocation>::iterator d1;  // Iterator for each dislocation
   std::vector<Dislocation>::iterator d2;  // Nested iterator
   std::vector<Stress>::iterator s;        // Iterator for the Stress

   Vector3d p;                             // Position vector

   s = this->dislocationStresses.begin();
   for (d1=this->dislocations.begin(); d1!=this->dislocations.end(); d1++)
     {
       *s = appliedStress;
       p = d1->getPosition();
       for (d2 = this->dislocations.begin(); d2!=this->dislocations.end(); d2++)
	 {
	   if (d1==d2)
	     {
	       continue;
	     }
	   else
	     {
	       *s = *s + d2->stressField(p, mu, nu);
	     }
	 }
     }
 }

 /**
  * @brief This function populates the STL vector container dislocationForces with the Peach-Koehler force experienced by each dislocation.
  * @details This function calculates the Peach-Koehler force experienced by each dislocation using the function Dislocation::forcePeachKoehler and the STL vector SlipPlane::dislocationStresses. The argument tau_crss is the Critical Resolved Shear Stress in Pa.
  * @param tau_crss Critical Resolved Shear Stress in Pa.
  */
 void SlipPlane::calculateDislocationForces (double tau_crss)
 {
   std::vector<Dislocation>::iterator d;  // Iterator for dislocations
   std::vector<Vector3d>::iterator f;     // Iterator for forces
   std::vector<Stress>::iterator s;       // Iterator for stresses

   s = this->dislocationStresses.begin();
   f = this->dislocationForces.begin();

   for (d = this->dislocations.begin(); d!=this->dislcoations.end(); d++)
     {
       f = d->forcePeachKoehler (*s, tau_crss);
       s++;
       f++;
     }
 }

 /**
  * @brief Calculates the velocities of dislocations and stores them in the std::vector container velocities.
  * @details The velocities of the dislocations are calculated and stored in the std::vector container called velocities. The velocities are calculated using the proportionality law between them and the Peach-Koehler force, using the drag coefficient B as the constant of proportionality.
  * param B The drag coefficient.
  */
 void SlipPlane::calculateVelocities (double B)
 {
   std::vector<Dislocation>::iterator d;  // Iterator for dislocations
   std::vector<Vector3d>::iterator f;     // Iterator for forces
   std::vector<Vector3d>::iterator v;     // Iterator for velocities

   d = this->dislocations.begin();
   f = this->dislocationForces.begin();
   v = this->dislocationVelocities.begin();

   while (v != this->dislocationVelocities.end())
     {
       if (d->isMobile())
	 {
	   *v = (*f)/B;
	 }
       d++;
       f++;
       v++;
     }
 }
