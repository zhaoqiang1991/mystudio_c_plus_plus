package com.example.myapplication.opengl;


import android.content.Context;


import static android.opengl.GLES20.glUseProgram;

import com.example.myapplication.opengl.utils.ShaderHelper;
import com.example.myapplication.opengl.utils.TextResourceReader;

public class ShaderProgram {

	// Uniform constants
	protected static final String U_MATRIX = "u_Matrix";
	protected static final String U_TEXTURE_UNIT = "u_TextureUnit";
	protected static final String U_TEXTURE_OES_UNIT = "u_TextureOESUnit";
	protected static final String U_STRENGTH = "u_Strength";
	protected static final String U_WIDTH_FACTOR = "u_imageWidthFactor";
	protected static final String U_HEIGHT_FACTOR = "u_imageHeightFactor";
	protected static final String U_TEXTURE_TRANSFORM = "u_textureTransform";
	protected static final String U_MVPMATRIX = "uMvpMatrix";
	protected static final String U_FLASHPARAM = "uflashParam";
	protected static final String U_BURRPARAM = "uBurrParam";
	protected static final String U_ALPHA = "u_alpha";
	protected static final String U_OFFSET = "u_Offset";

	protected static final String TexelWidthOffset = "texelWidthOffset";
	protected static final String TexelHeightOffset = "texelHeightOffset";

	// Attribute constants
	protected static final String A_POSITION = "a_Position";
	protected static final String A_COLOR = "a_Color";
	protected static final String A_TEXTURE_COORDINATES = "a_TextureCoordinates";

	// Shader program
	protected final int program;

	protected ShaderProgram(Context context, int vertexShaderResourceId,
							int fragmentShaderResourceId) {
		// Compile the shaders and link the program.
		program = ShaderHelper.buildProgram(
				TextResourceReader.readTextFileFromResource(
						context, vertexShaderResourceId),
				TextResourceReader.readTextFileFromResource(
						context, fragmentShaderResourceId));
	}

	public void useProgram() {
		// Set the current OpenGL shader program to this program.
		glUseProgram(program);
	}

}
