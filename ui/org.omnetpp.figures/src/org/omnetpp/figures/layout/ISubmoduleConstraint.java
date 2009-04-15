/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.figures.layout;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;

/**
 * @author rhornig
 * Submodule layout is using this interface
 */
public interface ISubmoduleConstraint {
	enum VectorArrangement {exact, row, column, matrix, ring};

	/**
	 * The center point of the main area (icon/shape) of a submodule figure.
	 * Returns <code>null</code> if setCenterLocation() was not called before. This
	 * means that the submodule was added to the parent recently and an auto-layout
	 * should be executed to place the submodule inside the compound module.
	 */
	public Point getCenterLocation();
	
	/**
	 * Sets the location of a submodule (the center point of its main area icon/shape).
	 * This method must store the currently set value which can be later returned by
	 * {@link #getCenterLocation()}. Setting it to <code>null</code> is allowed meaning 
	 * that the submodule location must be determined by the layouting algorithm.
	 * In addition this function MUST set the bounds of the figure correctly based on the
	 * size of the figure and the currently set center point.
	 */
	public void setCenterLocation(Point loc);
	
	/**
	 * Returns whether the submodule is pinned. If it is pinned, the centerLoaction should 
	 * have been set already (position is set in the display string)
	 */
	public boolean isFixedPosition();
	
	/**
	 * Identifies the vector this submodule belongs to.
	 */
	public Object getVectorIdentifier();
	
	/**
	 * The size of the submodule. This is used during the layouting process. This is NOT the same
	 * as the bounds of the figure, because the figure might draw range indicators, and additional 
	 * text annotation which is not treated as an "important" part of the figure. 
	 */
	public Dimension getSize();
	
	/**
	 * The size of the module vector. Returns 0 if it is not a module vector.
	 */
	public int getVectorSize();
	
	/**
	 * The index of the module in a vector (<vectorSIze())
	 */
	public int getVectorIndex();
	
	/**
	 * The type of the vector arrangement (exact, column, etc...) 
	 */
	public VectorArrangement getVectorArrangement();
	
	/**
	 * First argument to the vector arrangement.
	 */
	public int getLayoutPar1();
	
	/**
	 * Second argument to the vector arrangement.
	 */
	public int getLayoutPar2();

	/**
	 * Third argument to the vector arrangement.
	 */
	public int getLayoutPar3();
}
