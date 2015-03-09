//SpriteCol

SpritePos
SpriteSize
TexCoord
BackgroundTexSize
SpriteTexSize
OffsetBackground
BackgroundSize
Background
Sprite
Frame
FrameCol
FrameRow
Orientation
Foreground
ForegroundAlpha

// SpritePos : Ratio of Background Texture
float RatioPoster = BackgroundTexSize.y / BackgroundTexSize.x;
float RatioSoldierPosterY = SpriteTexSize.y / BackgroundTexSize.y;

float2 RatioSoldierPoster = float2(
	(BackgroundSize.x * SpriteSize.x) / (SpriteTexSize.x),
	(BackgroundSize.y * SpriteSize.y) / (SpriteTexSize.y * RatioPoster)
);

SpritePos = float2(
	(OffsetBackground.x + BackgroundSize.x * SpritePos.x * BackgroundTexSize.x) / BackgroundTexSize.x,
	(OffsetBackground.y + BackgroundSize.y * SpritePos.y * BackgroundTexSize.y) / BackgroundTexSize.y
);

SpriteSize = SpritePos + float2(
	SpriteSize.x * RatioPoster * RatioSoldierPoster.x / RatioSoldierPoster.y,
	SpriteSize.y
);

if (TexCoord.x > SpritePos.x && TexCoord.x <= SpriteSize.x
	&& TexCoord.y > SpritePos.y && TexCoord.y <= SpriteSize.y)
{
	float2 UV;

	if (Orientation > 0)
	{
		UV = float2(
			(TexCoord.x - SpritePos.x) / (SpriteSize.x - SpritePos.x),
			(TexCoord.y - SpritePos.y) / (SpriteSize.y - SpritePos.y)
		);
	}
	else
	{
		UV = float2(
			1 - (TexCoord.x - SpritePos.x) / (SpriteSize.x - SpritePos.x),
			(TexCoord.y - SpritePos.y) / (SpriteSize.y - SpritePos.y)
		);
	}
	float4 SpriteCol = Sprite.Sample(SpriteSampler, UV);
	if (SpriteCol.g > 0.75 && SpriteCol.r < 0.1 && SpriteCol.b < 0.1)
	{
		SpriteCol.g = 0;
		SpriteCol.a = 0;
	}
	else
	{
		SpriteCol.a = 1;
	}
	
	float4 BackgroundCol = Background.Sample(BackgroundSampler, TexCoord.xy);
	float4 ForegroundCol = Foreground.Sample(ForegroundSampler, TexCoord.xy);
	float4 FinalCol = float4(
		SpriteCol.rgb * SpriteCol.a + BackgroundCol.rgb * (1 - SpriteCol.a),
		SpriteCol.a * SpriteCol.a + BackgroundCol.a * (1 - SpriteCol.a)
	);
	ForegroundCol.a *= ForegroundAlpha;

	return float4(
		ForegroundCol.rgb * ForegroundCol.a + FinalCol.rgb * (1 - ForegroundCol.a),
		ForegroundCol.a * ForegroundCol.a + FinalCol.a * (1 - ForegroundCol.a)
	);
}
else
{
	float4 BackgroundCol = Background.Sample(BackgroundSampler, TexCoord.xy);
	float4 ForegroundCol = Foreground.Sample(ForegroundSampler, TexCoord.xy);
	ForegroundCol.a *= ForegroundAlpha;
	return float4(
		ForegroundCol.rgb * ForegroundCol.a + BackgroundCol.rgb * (1 - ForegroundCol.a),
		ForegroundCol.a * ForegroundCol.a + BackgroundCol.a * (1 - ForegroundCol.a)
	);
}
