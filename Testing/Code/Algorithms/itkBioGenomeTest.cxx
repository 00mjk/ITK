/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkBioGenomeTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include <iostream>

#include "itkBioGenome.h"


int itkBioGenomeTest( int, char * [] )
{
   itk::bio::Genome genome;


   genome.InsertGene("Tubulin");
   genome.InsertGene("Cyclin");

   const double tolerance = 1e-30;

   const double inLevel = 0.74;
   genome.SetExpressionLevel("Cyclin",inLevel);

   const double outLevel = genome.GetExpressionLevel("Cyclin");
   
   if( fabs( inLevel - outLevel ) > tolerance )
     {
     std::cerr << "Error in SetExpressionLevel()/GetExpressionLevel()" << std::endl;
     return EXIT_FAILURE;
     }
   
   genome.KnockOutGene("Cyclin");

   if( fabs( genome.GetExpressionLevel("Cyclin") ) > tolerance )
     {
     std::cerr << "Error in KnockOutGene()/GetExpressionLevel()" << std::endl;
     return EXIT_FAILURE;
     }
   
   const double value = 3.0;
   const double threshold = 2.0;
   const double slant = 5.0;
   
   const double sigmoid = itk::bio::Genome::Sigmoide( threshold, slant, value );

   const double expectedSigmoid = atan(( value - threshold ) / slant ) / 3.1416 + 0.5001;

   if( fabs( sigmoid - expectedSigmoid ) > tolerance )
     {
     std::cerr << "Error in Sigmoid()" << std::endl;
     return EXIT_FAILURE;
     }
   

   const double cyclinLevel  = 3.45;
   const double tubulinLevel = 2.79;
   
   genome.SetExpressionLevel("Cyclin" ,cyclinLevel );
   genome.SetExpressionLevel("Tubulin",tubulinLevel);

   itk::bio::Genome genome2;

   genome2.Copy( genome );

   if( fabs( genome.GetExpressionLevel("Tubulin") -
             genome2.GetExpressionLevel("Tubulin") ) > tolerance )
     {
     std::cerr << "Error in Copy()" << std::endl;
     return EXIT_FAILURE;
     }

   if( fabs( genome.GetExpressionLevel("Cyclin") -
             genome2.GetExpressionLevel("Cyclin") ) > tolerance )
     {
     std::cerr << "Error in Copy()" << std::endl;
     return EXIT_FAILURE;
     }
   
   std::cout << "Test Passed !" << std::endl;
   return EXIT_SUCCESS;
}











