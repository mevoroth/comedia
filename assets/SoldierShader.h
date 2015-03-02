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

// SpritePos : Ratio of Background Texture
float2 RatioSoldierPoster = float2(
	BackgroundSize.x / (SpriteTexSize.x),
	BackgroundSize.y / (SpriteTexSize.y)
);

SpritePos = float2(
	(OffsetBackground.x + BackgroundSize.x * SpritePos.x * BackgroundTexSize.x) / BackgroundTexSize.x,
	(OffsetBackground.y + BackgroundSize.y * SpritePos.y * BackgroundTexSize.y) / BackgroundTexSize.y
);

SpriteSize = SpritePos + float2(
	SpriteSize.x * BackgroundSize.x / (BackgroundTexSize.x * RatioSoldierPoster.x),
	SpriteSize.y * BackgroundSize.y / (BackgroundTexSize.y * RatioSoldierPoster.y)
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
	if (SpriteCol.r == 0 && SpriteCol.g == 0 && SpriteCol.b == 0)
	{
		SpriteCol.a = 0;
	}
	
	float4 BackgroundCol = Background.Sample(BackgroundSampler, TexCoord.xy);

	return float4(
		SpriteCol.rgb * SpriteCol.a + BackgroundCol.rgb * (1 - SpriteCol.a),
		SpriteCol.a * SpriteCol.a + BackgroundCol.a * (1 - SpriteCol.a)
	);
}
else
{
	return Background.Sample(BackgroundSampler, TexCoord.xy);
}
