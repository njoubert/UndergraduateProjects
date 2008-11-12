package org.red5.io.composite.impl;

import java.io.File;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.red5.io.ITagReader;
import org.red5.server.api.IScope;
import java.util.Collection;
import org.red5.io.composite.impl.UrlResolver;
import java.io.IOException;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.xpath.XPath;
import javax.xml.xpath.XPathConstants;
import javax.xml.xpath.XPathExpression;
import javax.xml.xpath.XPathExpressionException;
import javax.xml.xpath.XPathFactory;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;
import java.io.InputStream;
import java.net.URL;

public class CompositeMapCreator {
	
	protected static Log log = LogFactory.getLog(CompositeMapCreator.class.getName());
	
	private UrlResolver urlResolver;
	
	private IScope scope;
	
	public CompositeMapCreator(UrlResolver resolver, IScope scope) {
		this.urlResolver = resolver;
		this.scope = scope;
	}
	
	/**
	 * Here we use the UrlResolver and static classes to query a REST-based server for XML to 
	 * describe how to create this composite file. 
	 * @param url
	 * @param elements
	 * @return
	 */
	public boolean readElementsFromURL(URL url, Collection<CompositeElement> elements) {
		InputStream inputReader  = urlResolver.getXML(url);
		
		if (inputReader == null) {
			log.error("Error in pulling XML from server!");
			return false;
		}
		Document dom;
		DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
		try {
			DocumentBuilder db = dbf.newDocumentBuilder();
			dom = db.parse(inputReader);
		} catch (ParserConfigurationException pce) {
			log.error("Could not parse XML file.", pce);
			return false;
		} catch (SAXException se) {
			log.error("Could not parse XML file.", se);
			return false;
		} catch (IOException ioe) {
			log.error("Could not parse XML file.", ioe);
			return false;
		}
		
		return readDOM(dom, elements);
	}
	

	/**
	 * Given a file and a (probably) empty list, this method 
	 * fills the list with CompositeElement objects
	 * from the XML in the file.
	 * @param file
	 * @param elements
	 * @return true if successful.
	 */
	public boolean readElementsFile(File file, Collection<CompositeElement> elements) {
		Document dom;
		DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
		try {
			DocumentBuilder db = dbf.newDocumentBuilder();
			dom = db.parse(file);
		} catch (ParserConfigurationException pce) {
			log.error("Could not parse XML file.", pce);
			return false;
		} catch (SAXException se) {
			log.error("Could not parse XML file.", se);
			return false;
		} catch (IOException ioe) {
			log.error("Could not parse XML file.", ioe);
			return false;
		}
		
		return readDOM(dom, elements);
		
	}
	
	/**
	 * Given a list of elements and a file to save to, this method 
	 * saves the elements into a XML structure on disk.
	 * @param elements
	 * @param file
	 * @return
	 */
	public static boolean saveElementsToFile(File file, Collection<CompositeElement> elements) {
		
		throw new UnsupportedOperationException();
		
		//TODO: Implement this!
		
		/*
		Document dom = createDOM(elements);
		if (dom == null)
			return false;
		OutputFormat format = new OutputFormat(dom);
		format.setIndenting(true);
		try {
			XMLSerializer serializer = new XMLSerializer(new FileOutputStream(file), format);
			serializer.serialize(dom);
			return true;
		} catch (IOException err) {
			log.error("could not write XML data", err);
		}
		return false; */
	}
	
	/**
	 * Creates a DOM that can be written out to XML from the given list of composite elements.
	 * @param elements
	 * @return null if something fails.
	 */
	private static Document createDOM(Collection<CompositeElement> elements) {
		
		//TODO: This is NOT completed since we dont have any way to get the url/path of the readers.
		
		Document dom;
		DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
		try {
			DocumentBuilder db = dbf.newDocumentBuilder();
			dom = db.newDocument();

			Element root = (Element) dom.createElement("ResultSet");
			root.setAttribute("totalResultsAvailable", Integer.toString(elements.size()));
			root.setAttribute("totalResultsReturned", Integer.toString(elements.size()));
			root.setAttribute("firstResultPosition", "1");
			dom.appendChild(root);
			
			Element result;
			Element url;
			Element start;
			Element end;
			for (CompositeElement el : elements) {
				result = dom.createElement("Result");
				url = dom.createElement("Url");
				start = dom.createElement("Start");
				end = dom.createElement("End");
				
				url.appendChild(dom.createTextNode(el.getFileLocation()));
				start.appendChild(dom.createTextNode(Double.toString((double)el.getInPointTime() / 1000.0)));
				end.appendChild(dom.createTextNode(Double.toString((double)el.getOutPointTime() / 1000.0)));
				
				result.appendChild(url);
				result.appendChild(start);
				result.appendChild(end);
				root.appendChild(result);
			}
			
			dom.getDocumentElement().normalize();
			return dom;
			
		} catch (ParserConfigurationException pce) {
			log.error("Could not create XML DOM!");
			log.error(pce);
			return null;
		}

	}
	
	/**
	 * Reads the given DOM into the given List
	 * @param dom
	 * @return false if something fails.
	 */
	private boolean readDOM(Document dom, Collection<CompositeElement> elements) {
		Element root = dom.getDocumentElement();
		if (!"ResultSet".equals(root.getNodeName()))
			return false;
		
		XPathFactory factory = XPathFactory.newInstance();
		XPath xpath = factory.newXPath();
		NodeList results;
		try {
			XPathExpression xexpr = xpath.compile("/ResultSet/Result");
			results = (NodeList) xexpr.evaluate(dom, XPathConstants.NODESET);
		} catch (XPathExpressionException err) {
			log.error("Could not compile xpath expression and get list of elements. ", err);
			return false;
		}
		int length = results.getLength();
		Node node1, node2;
		NodeList nodeInfo;
		ITagReader reader = null;
		double start = 0, end = 0;
		long correction = 0;
		CompositeElement el;
		CompositeElement prevEl = null;
		for (int i = 0; i < length; i++) {
			
			node1 = results.item(i);
			
			nodeInfo = node1.getChildNodes();
			for (int j = 0; j < nodeInfo.getLength(); j++) {
				node2 = nodeInfo.item(j);
				if (node2.getNodeName() == "Url")
					reader = UrlResolver.resolveUrl(scope, node2.getFirstChild().getNodeValue());
				else if (node2.getNodeName() == "Start")
					start = Double.parseDouble(node2.getFirstChild().getNodeValue()) * 1000;
				else if (node2.getNodeName() == "End")
					end = Double.parseDouble(node2.getFirstChild().getNodeValue()) * 1000;
			}
			
			if (reader != null) {
				el = new CompositeElement(reader, (long) start, (long) end);
				if (el.isValid()) {
					
					correction = -(el.getInPointTime());
					if (prevEl != null) {
						correction = correction + prevEl.getOutPointTime() + prevEl.getCorrection();
					}
					el.setCorrection(correction);
					elements.add(el);
					prevEl = el;
				}
			}
			
		}
		
		return true;
	}
	
	
	
	
	
}
