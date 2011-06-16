/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: Mesh2.cxx,v $
  Language:  C++
  Date:      $Date: 2009-03-17 21:11:44 $
  Version:   $Revision: 1.18 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

//  Software Guide : BeginLatex
//
//  A \doxygen{Mesh} can contain a variety of cell types. Typical cells are
//  the \doxygen{LineCell}, \doxygen{TriangleCell},
//  \doxygen{QuadrilateralCell} and \doxygen{TetrahedronCell}. Additional
//  flexibility is provided for managing cells at the price of a bit more of
//  complexity than in the case of point management.
//
//  The following code creates a polygonal line in order to illustrate the
//  simplest case of cell management in a Mesh. The only cell type used here is
//  the LineCell. The header file of this class has to be included.
//
//  \index{itk::LineCell!Header}
//
//  Software Guide : EndLatex 


#include "itkMesh.h"

// Software Guide : BeginCodeSnippet
#include "itkLineCell.h"
// Software Guide : EndCodeSnippet


int main(int, char *[])
{
  typedef float                             PixelType;
  typedef itk::Mesh< PixelType, 3 >         MeshType;

  //  Software Guide : BeginLatex
  //  
  //  In order to be consistent with the Mesh, cell types have to be configured
  //  with a number of custom types taken from the mesh traits. The set of
  //  traits relevant to cells are packaged by the Mesh class into the
  //  \code{CellType} trait. This trait needs to be passed to the actual cell
  //  types at the moment of their instantiation. The following line shows how
  //  to extract the Cell traits from the Mesh type.
  //
  //  \index{itk::Mesh!CellType}
  //  \index{itk::Mesh!traits}
  //
  //  Software Guide : EndLatex 

  // Software Guide : BeginCodeSnippet
  typedef MeshType::CellType                CellType;
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  //
  //  The LineCell type can now be instantiated using the traits
  //  taken from the Mesh.  
  //
  //  \index{itk::LineCell!Instantiation}
  //
  //  Software Guide : EndLatex 

  // Software Guide : BeginCodeSnippet
  typedef itk::LineCell< CellType >         LineType;
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  //
  //  The main difference in the way cells and points are managed by
  //  the Mesh is that points are stored by copy on the
  //  PointsContainer while cells are stored in the CellsContainer
  //  using pointers. The reason for using pointers is that cells use
  //  C++ polymorphism on the mesh. This means that the mesh is only
  //  aware of having pointers to a generic cell which is the base
  //  class of all the specific cell types. This architecture makes it
  //  possible to combine different cell types in the same
  //  mesh. Points, on the other hand, are of a single type and have a
  //  small memory footprint, which makes it efficient to copy them
  //  directly into the container.
  //
  //  \index{itk::Cell!CellAutoPointer}
  //  \index{itk::Mesh!CellAutoPointer}
  //  \index{CellAutoPointer}
  //  \index{itk::AutoPointer}
  //
  //  Managing cells by pointers add another level of complexity to the Mesh
  //  since it is now necessary to establish a protocol to make clear who is
  //  responsible for allocating and releasing the cells' memory. This protocol
  //  is implemented in the form of a specific type of pointer called the
  //  \code{CellAutoPointer}. This pointer, based on the \doxygen{AutoPointer},
  //  differs in many respects from the SmartPointer. The CellAutoPointer has an
  //  internal pointer to the actual object and a boolean flag that indicates
  //  if the CellAutoPointer is responsible for releasing the cell memory
  //  whenever the time comes for its own destruction. It is said that a
  //  \code{CellAutoPointer} \emph{owns} the cell when it is responsible for
  //  its destruction. Many CellAutoPointer can point to the same cell but at
  //  any given time, only \textbf{one} CellAutoPointer can own the cell. 
  //
  //  The \code{CellAutoPointer} trait is defined in the MeshType and can be
  //  extracted as illustrated in the following line.
  //
  //  Software Guide : EndLatex 
  
  // Software Guide : BeginCodeSnippet
  typedef CellType::CellAutoPointer         CellAutoPointer;
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  //
  //  Note that the CellAutoPointer is pointing to a generic cell type. It is
  //  not aware of the actual type of the cell, which can be for example
  //  LineCell, TriangleCell or TetrahedronCell. This fact will influence the
  //  way in which we access cells later on.
  //
  //  At this point we can actually create a mesh and insert some points on it.
  //
  //  \index{itk::Mesh!New()}
  //  \index{itk::Mesh!SetPoint()}
  //  \index{itk::Mesh!PointType}
  //
  //  Software Guide : EndLatex 

  // Software Guide : BeginCodeSnippet
  MeshType::Pointer  mesh = MeshType::New();

  MeshType::PointType p0;
  MeshType::PointType p1;
  MeshType::PointType p2;

  p0[0] = -1.0; p0[1] = 0.0; p0[2] = 0.0;
  p1[0] =  1.0; p1[1] = 0.0; p1[2] = 0.0;
  p2[0] =  1.0; p2[1] = 1.0; p2[2] = 0.0;

  mesh->SetPoint( 0, p0 );
  mesh->SetPoint( 1, p1 );
  mesh->SetPoint( 2, p2 );
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  //
  //  The following code creates two CellAutoPointers and initializes
  //  them with newly created cell objects. The actual cell type
  //  created in this case is LineCell. Note that cells are
  //  created with the normal \code{new} C++ operator. The
  //  CellAutoPointer takes ownership of the received pointer by using
  //  the method \code{TakeOwnership()}. Even though this may seem
  //  verbose, it is necessary in order to make it explicit from the
  //  code that the responsibility of memory release is assumed by the
  //  AutoPointer.
  //
  //  \index{itk::AutoPointer!TakeOwnership()}
  //  \index{CellAutoPointer!TakeOwnership()}
  //  \index{CellType!creation}
  //
  //  Software Guide : EndLatex 

  // Software Guide : BeginCodeSnippet
  CellAutoPointer line0;
  CellAutoPointer line1;

  line0.TakeOwnership(  new LineType  );
  line1.TakeOwnership(  new LineType  );
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  //
  //  The LineCells should now be associated with points in the mesh. This is
  //  done using the identifiers assigned to points when they were inserted
  //  in the mesh. Every cell type has a specific number of points that must
  //  be associated with it.\footnote{Some cell types like polygons have a
  //  variable number of points associated with them.} For example a
  //  LineCell requires two points, a TriangleCell
  //  requires three and a TetrahedronCell requires four. Cells use
  //  an internal numbering system for points. It is simply an index in the
  //  range $\{0,NumberOfPoints-1\}$. The association of points and cells is
  //  done by the \code{SetPointId()} method which requires the user to
  //  provide the internal index of the point in the cell and the
  //  corresponding PointIdentifier in the Mesh. The internal cell index is
  //  the first parameter of \code{SetPointId()} while the mesh
  //  point-identifier is the second.
  //
  //  Software Guide : EndLatex 


  // Software Guide : BeginCodeSnippet
  line0->SetPointId( 0, 0 ); // line between points 0 and 1
  line0->SetPointId( 1, 1 );

  line1->SetPointId( 0, 1 ); // line between points 1 and 2
  line1->SetPointId( 1, 2 );
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  //
  //  Cells are inserted in the mesh using the \code{SetCell()} method. It
  //  requires an identifier and the AutoPointer to the cell. The Mesh will
  //  take ownership of the cell to which the AutoPointer is pointing. This is
  //  done internally by the \code{SetCell()} method. In this way, the
  //  destruction of the CellAutoPointer will not induce the destruction of the
  //  associated cell.
  //
  //  \index{itk::Mesh!SetCell()}
  //  \index{SetCell()!itk::Mesh}
  //  \index{itk::Mesh!Inserting cells}
  //
  //  Software Guide : EndLatex 

  // Software Guide : BeginCodeSnippet
  mesh->SetCell( 0, line0 );
  mesh->SetCell( 1, line1 );
  // Software Guide : EndCodeSnippet

  //  Software Guide : BeginLatex
  //
  //  After serving as an argument of the \code{SetCell()} method, a
  //  CellAutoPointer no longer holds ownership of the cell. It is important
  //  not to use this same CellAutoPointer again as argument to
  //  \code{SetCell()} without first securing ownership of another cell.
  //
  //  Software Guide : EndLatex 

  std::cout << "Points = " << mesh->GetNumberOfPoints() << std::endl;

  //  Software Guide : BeginLatex
  //
  //  The number of Cells currently inserted in the mesh can be queried with
  //  the \code{GetNumberOfCells()} method.
  //
  //  \index{itk::Mesh!GetNumberOfCells()}
  //
  //  Software Guide : EndLatex 

  // Software Guide : BeginCodeSnippet
  std::cout << "Cells  = " << mesh->GetNumberOfCells()  << std::endl;
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  //
  //  In a way analogous to points, cells can be accessed using Iterators to
  //  the CellsContainer in the mesh. The trait for the cell iterator can be
  //  extracted from the mesh and used to define a local type.
  //
  //  Software Guide : EndLatex 

  // Software Guide : BeginCodeSnippet
  typedef MeshType::CellsContainer::Iterator  CellIterator;
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  //
  //  Then the iterators to the first and past-end cell in the mesh can be
  //  obtained respectively with the \code{Begin()} and \code{End()} methods of
  //  the CellsContainer. The CellsContainer of the mesh is returned by the
  //  \code{GetCells()} method.
  //
  //  \index{itk::Mesh!Iterating cells}
  //  \index{itk::Mesh!GetCells()}
  //  \index{CellsContainer!Begin()}
  //  \index{CellsContainer!End()}
  //
  //  Software Guide : EndLatex 

  // Software Guide : BeginCodeSnippet
  CellIterator  cellIterator = mesh->GetCells()->Begin();  
  CellIterator  end          = mesh->GetCells()->End();  
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  //
  //  Finally a standard loop is used to iterate over all the cells. Note the
  //  use of the \code{Value()} method used to get the actual pointer to the
  //  cell from the CellIterator. Note also that the values returned are
  //  pointers to the generic CellType. These pointers have to be down-casted
  //  in order to be used as actual LineCell types. Safe down-casting is
  //  performed with the \code{dynamic\_cast} operator which will throw an
  //  exception if the conversion cannot be safely performed.
  //
  //  \index{down casting}
  //  \index{CellIterator!Value()}
  //  \index{CellIterator!increment}
  //  \index{itk::Mesh!CellType casting}
  //  \index{Print()}
  //  \index{CellType!Print()}
  //  \index{CellType!GetNumberOfPoints()}
  //  \index{LineCell!Print()}
  //  \index{LineCell!GetNumberOfPoints()}
  //
  //  Software Guide : EndLatex 


  // Software Guide : BeginCodeSnippet
  while( cellIterator != end ) 
    {
    MeshType::CellType * cellptr = cellIterator.Value();
    LineType * line = dynamic_cast<LineType *>( cellptr );
    std::cout << line->GetNumberOfPoints() << std::endl;
    ++cellIterator;
    }
  // Software Guide : EndCodeSnippet

  return 0;
}
