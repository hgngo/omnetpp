/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.properties;

import java.util.ArrayList;
import java.util.Collections;
import java.util.EnumSet;
import java.util.List;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource2;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.omnetpp.ned.core.INedResources;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.editor.graph.properties.util.DelegatingPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.DisplayPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.MergedPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.ParameterListPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.TypePropertySource;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class ConnectionPropertySource extends MergedPropertySource {

    // Display property source
    protected static class ConnectionDisplayPropertySource extends DisplayPropertySource {
        protected static IPropertyDescriptor[] propertyDescArray;
        protected ConnectionElementEx model;

        public ConnectionDisplayPropertySource(ConnectionElementEx model) {
            super(model);
            this.model = model;
            // define which properties should be displayed in the property sheet
            // we do not support all properties currently, just color, width and style
            supportedProperties.addAll(EnumSet.range(DisplayString.Prop.ROUTING_CONSTRAINT,
                    								 DisplayString.Prop.CONNECTION_STYLE));

            supportedProperties.addAll(EnumSet.range(DisplayString.Prop.TEXT, DisplayString.Prop.TEXTCOLOR));
            supportedProperties.add(DisplayString.Prop.TOOLTIP);
        }

    }

    // Connection specific properties
    protected static class BasePropertySource implements IPropertySource2 {
        public enum Prop { SrcGate, DestGate }
        protected IPropertyDescriptor[] descriptors;
        protected ConnectionElementEx model;

        public BasePropertySource(ConnectionElementEx connectionNodeModel) {
            model = connectionNodeModel;

            // set up property descriptors
            PropertyDescriptor srcGateProp = new PropertyDescriptor(Prop.SrcGate, "source-gate");
            srcGateProp.setCategory("Base");
            srcGateProp.setDescription("The source gate of the connection - (read only)");

            PropertyDescriptor destGateProp = new PropertyDescriptor(Prop.DestGate, "dest-gate");
            destGateProp.setCategory("Base");
            destGateProp.setDescription("The destination gate of the connection - (read only)");

            descriptors = new IPropertyDescriptor[] { srcGateProp, destGateProp };
        }

        public Object getEditableValue() {
            return this;
        }

        public IPropertyDescriptor[] getPropertyDescriptors() {
            return descriptors;
        }

        public Object getPropertyValue(Object propName) {
            if (Prop.SrcGate.equals(propName))
                return model.getSrcGateFullyQualified();

            if (Prop.DestGate.equals(propName))
                return model.getDestGateFullyQualified();

            return null;
        }

        public void setPropertyValue(Object propName, Object value) {
        }

        public boolean isPropertySet(Object propName) {
            return false;
        }

        public void resetPropertyValue(Object propName) {
        }

        public boolean isPropertyResettable(Object propName) {
            return false;
        }
    }

    // constructor
    public ConnectionPropertySource(final ConnectionElementEx connectionNodeModel) {
        super(connectionNodeModel);
        // create a nested displayPropertySources
        mergePropertySource(new BasePropertySource(connectionNodeModel));
        // type
        mergePropertySource(new TypePropertySource(connectionNodeModel) {
            @Override
            protected List<String> getPossibleTypeValues() {
                INedResources res = NedResourcesPlugin.getNedResources();
                INedTypeLookupContext context = connectionNodeModel.getEnclosingLookupContext();
                List<String> channelNames = new ArrayList<String>(res.getVisibleTypeNames(context, INedResources.CHANNEL_FILTER));
                channelNames.addAll(res.getInvisibleTypeNames(context, INedResources.CHANNEL_FILTER));
                Collections.sort(channelNames);
                return channelNames;
            }
            protected List<String> getPossibleLikeTypeValues() {
                INedResources res = NedResourcesPlugin.getNedResources();
                CompoundModuleElementEx context = connectionNodeModel.getCompoundModule();
                List<String> channelInterfaceNames = new ArrayList<String>(res.getVisibleTypeNames(context, INedResources.CHANNELINTERFACE_FILTER));
                channelInterfaceNames.addAll(res.getInvisibleTypeNames(context, INedResources.CHANNELINTERFACE_FILTER));
                Collections.sort(channelInterfaceNames);
                return channelInterfaceNames;
            }
        });
        mergePropertySource(new ConnectionDisplayPropertySource(connectionNodeModel));
        mergePropertySource(new DelegatingPropertySource(
                new ParameterListPropertySource(connectionNodeModel),
                ParameterListPropertySource.CATEGORY,
                ParameterListPropertySource.DESCRIPTION));

    }
}

