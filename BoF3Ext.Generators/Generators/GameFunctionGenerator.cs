using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.CSharp.Syntax;
using Scriban;
using Scriban.Runtime;
using System;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace BoF3Ext.Generators;


[Conditional("SOURCE_GENERATOR")]
[AttributeUsage(AttributeTargets.Method, Inherited = false)]
#pragma warning disable CS9113 // Parameter is unread.
public sealed class GameFunctionAttribute(
	uint address,
	bool hookable = false,
	CallingConvention callingConvention = CallingConvention.Cdecl
) : Attribute;
#pragma warning restore CS9113 // Parameter is unread.


[Generator(LanguageNames.CSharp)]
public sealed class GameFunctionGenerator : IIncrementalGenerator {
	static void GenerateGameFunction(SourceProductionContext ctx, (AttributeData attr, IMethodSymbol sym) info) {
		ctx.CancellationToken.ThrowIfCancellationRequested();

		(var attr, var sym) = info;

		var template = Template.Parse(
			EmbeddedResources.ReadAllText("BoF3Ext.Generators.Templates.GameFunctionTemplate.scriban")
		);

		if (template.HasErrors)
			throw new InvalidOperationException(String.Join("\n", template.Messages));

		var globals = new ScriptObject();

		globals.Import(new {
			address = (uint)attr.ConstructorArguments[0].Value!,
			hookable = (bool)attr.ConstructorArguments[1].Value!,
			callingConvention = (CallingConvention)(int)attr.ConstructorArguments[2].Value! switch {
				CallingConvention.Cdecl => "Cdecl",
				CallingConvention.StdCall => "Stdcall",
				_ => throw new InvalidOperationException("Unsupported calling convention")
			},
			sym
		}, renamer: member => member.Name);

		var context = new SymbolTemplateContext {
			MemberRenamer = member => member.Name
		};

		context.PushGlobal(globals);

		var source = template.Render(context);

		source = CSharpSyntaxTree.ParseText(source, cancellationToken: ctx.CancellationToken)
			.GetRoot(ctx.CancellationToken)
			.NormalizeWhitespace()
			.ToFullString();

		ctx.AddSource($"{sym.ToDisplayString()}.GameFunction.g.cs", source);
	}


	public void Initialize(IncrementalGeneratorInitializationContext context) {
		var methods = context.SyntaxProvider.ForAttributeWithMetadataName(
			"BoF3Ext.Generators.GameFunctionAttribute",
			(node, _) => node is MethodDeclarationSyntax,
			(context, _) => (context.Attributes[0], (IMethodSymbol)context.TargetSymbol)
		);

		context.RegisterSourceOutput(methods, GenerateGameFunction);
	}
}
