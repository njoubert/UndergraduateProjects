package org.red5.io.composite.impl;

import java.io.File;
import java.io.IOException;
import org.red5.io.BaseStreamableFileService;
import org.red5.io.IStreamableFile;
import org.red5.io.composite.ICompositeFileService;
import org.red5.io.object.Deserializer;
import org.red5.io.object.Serializer;

/**
 * A CompositeFileServiceImpl sets up the service for creating composite
 * files and hands out the correct CompositeFile object to its callers.
 * 
 * @author njoubert@yahoo-inc.com
 *
 */
public class CompositeFileService extends BaseStreamableFileService implements ICompositeFileService {

    private Serializer serializer;

    private Deserializer deserializer;
    
	@Override
	public String getPrefix() {
		return "pxml";
	}
	
	@Override
	public String getExtension() {
		return ".pxml";
	}
	
	/** 
     * {@inheritDoc}
	 */
	public void setSerializer(Serializer serializer) {
		this.serializer = serializer;

	}

	/** {@inheritDoc}
	 */
	public void setDeserializer(Deserializer deserializer) {
		this.deserializer = deserializer;

	}

	@Override
	public IStreamableFile getStreamableFile(File file) throws IOException {
		return new CompositeFile(file);
	}


    public Serializer getSerializer() {
		return serializer;
	}

    public Deserializer getDeserializer() {
		return deserializer;
	}
}
